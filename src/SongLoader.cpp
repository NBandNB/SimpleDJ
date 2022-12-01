#include <thread>
#include <curl/curl.h>
#include <iostream>
#include "SongLoader.h"
//#include "FLAC++/metadata.h"

SongLoader::SongLoader(const std::filesystem::path& directory)
    : directory(directory),
    mutex(),
    songs(),
    downloadQueue(),
    deleteQueue(),
    locked(false)
{
    if(!std::filesystem::exists(directory)){
        std::filesystem::create_directory(directory);
    }
    if(!std::filesystem::exists(directory / "songs.csv")){
        std::ofstream(directory / "songs.csv").close();
    }
    std::ifstream file = std::ifstream(directory / "songs.csv", std::ios::in);
    if(!file.is_open())
        throw std::runtime_error("Could not open songs.csv");
    for(std::string line; getline(file, line);){
        std::string id = line.substr(0, line.find(','));
        line = line.substr(line.find(',') + 1);
        std::string name = line.substr(0, line.find(','));
        line = line.substr(line.find(',') + 1);
        std::string author = line.substr(0, line.find(','));

        std::ifstream songStream = std::ifstream(directory / (id + ".wav"));
        std::ifstream imageStream = std::ifstream(directory / (id + ".jpg"));
        QImage image = QImage(QString::fromStdString((directory / (id + ".jpg")).string()));
        if(!songStream.is_open() || !imageStream.is_open()) {
            songs.emplace(QString::fromStdString(id), std::make_shared<Song>(QString::fromStdString(name), QString::fromStdString(author), image, QString::fromStdString(id), false));
            continue;
        }
        songs.emplace(QString::fromStdString(id), std::make_shared<Song>(QString::fromStdString(name), QString::fromStdString(author), image, QString::fromStdString(id), true));
        songIds.push_back(id);
    }
    file.close();
    curl_global_init(CURL_GLOBAL_ALL);
}

SongLoader::~SongLoader() {
    curl_global_cleanup();
}

void SongLoader::download(const std::shared_ptr<Song>& song) {
    std::unique_lock lock(mutex);
    downloadQueue.emplace(song->getId(), songs.contains(song->getId()));
    if(!songs.contains(song->getId())){
        songs.emplace(song->getId(), song);
    }
}

void SongLoader::downloadLoop(){
    std::fstream file = std::fstream(directory / "songs.csv", std::ios::out | std::ios::in | std::ios::app);
    while(true) {
        std::unique_lock lock(mutex);
        while(!deleteQueue.empty()){
            QString id = deleteQueue.front();
            deleteQueue.pop();
            songs.at(id)->setDownloaded(false);
            songs.at(id)->setImage(QImage());
            songs.at(id)->setImageDownloaded(false);
            songIds.remove(id.toStdString());
            lock.unlock();
            deleteSongSignal(id);
            if(std::remove((directory / (id.toStdString() + ".wav")).string().c_str()) &&
            std::remove((directory / (id.toStdString() + ".jpg")).string().c_str())) {
                lock.lock();
                deleteQueue.push(id);
                lock.unlock();
            }
            lock.lock();
        }
        if(downloadQueue.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        QString id = downloadQueue.front().first;
        bool exists = downloadQueue.front().second;
        downloadQueue.pop();
        lock.unlock();
        if(isDownloaded(id)) {
            lock.unlock();
            continue;
        }
        std::filesystem::path path = directory / (id.toStdString() + ".wav");
        CURL* curl = curl_easy_init();
        QImage image;
        if(curl) {
            FILE *fp = fopen((directory / (id.toStdString() + ".jpg")).string().c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, ("https://i.ytimg.com/vi/" + id.toStdString() + "/maxresdefault.jpg").c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            if(curl_easy_perform(curl) != CURLE_OK)
                std::cout << "curl_easy_perform() failed\n";
            curl_easy_cleanup(curl);
            fclose(fp);
            image = QImage(QString::fromStdString((directory / (id.toStdString() + ".jpg")).string()));
        }
        else {
            std::cout << "curl_easy_init() failed\n";
            image = QImage();
        }
        lock.lock();
        songs.at(id)->setImage(image);
        songs.at(id)->setImageDownloaded();
        lock.unlock();
        // ACTUALLY DOWNLOAD IT HERE
        int result;
        do{
            result = system(("youtube-dl -f bestaudio --extract-audio --audio-format wav --output \"" + path.parent_path().string() + (const char)std::filesystem::path::preferred_separator + id.toStdString() + ".%(ext)s\" " + id.toStdString()).c_str());
        } while(result != 0);

        lock.lock();
        if(!file.is_open())
            throw std::runtime_error("Could not open songs.csv");
        if(!exists)
            file << (id.toStdString() + ',' + songs.at(id)->getName().toStdString() + ',' + songs.at(id)->getAuthor().toStdString()) << std::endl;
        songs.at(id)->setDownloaded();
        songIds.push_back(id.toStdString());
        lock.unlock();
        downloadCompleteSignal(id);
    }
}

void SongLoader::deleteSong(const QString& id){
    std::unique_lock lock(mutex);
    deleteQueue.push(id);
}

std::shared_ptr<Song> SongLoader::getRand() {
    std::shared_lock lock(mutex);
    if(songIds.empty()) {
        return nullptr;
    }
    int rand = std::rand() % songIds.size();
    auto it = songIds.begin();
    std::advance(it, rand);
    return songs.at(QString::fromStdString(*it));
}

std::shared_ptr<Song> SongLoader::getById(const QString &id){
    std::shared_lock lock(mutex);
    return songs.at(id);
}

bool SongLoader::isDownloaded(const QString &id) {
    std::shared_lock lock(mutex);
    if(songs.contains(id))
        return songs.at(id)->getDownloaded();
    return false;
}

bool SongLoader::containsSong(const QString &id) {
    std::shared_lock lock(mutex);
    return songs.contains(id);
}

QString SongLoader::getSongPath(const QString &id) {
    return QString::fromStdString((directory / (id.toStdString() + ".wav")).string());
}

bool SongLoader::isLocked() const{
    return locked;
}

void SongLoader::setPin(const QString& newPin){
    pin = newPin;
}

bool SongLoader::unlock(const QString& pinAttempt){
    if(pinAttempt == pin) {
        if (locked) {
            locked = false;
            unlockedSignal();
        }
        return locked;
    }
    else return locked;
}

void SongLoader::lock(){
    if(!locked){
        locked = true;
        lockedSignal();
    }
    else
        locked = true;
}

bool SongLoader::hasDownloaded() {
    std::shared_lock lock(mutex);
    return !songIds.empty();
}
