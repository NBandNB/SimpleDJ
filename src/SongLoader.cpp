#include <thread>
#include <curl/curl.h>
#include <iostream>
#include "SongLoader.h"
//#include "FLAC++/metadata.h"

SongLoader::SongLoader(const std::filesystem::path& directory)
    : directory(directory),
    mutex(),
    songs(),
    downloadQueue()
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
        if(!file.is_open())
            throw std::runtime_error("Could not open " + id + ".wav");
        QImage image = QImage(QString::fromStdString((directory / (id + ".jpg")).string()));
        songs.insert(std::make_pair(QString::fromStdString(id), std::make_shared<Song>(QString::fromStdString(name), QString::fromStdString(author), image, QString::fromStdString(id), true)));
    }
    file.close();
    curl_global_init(CURL_GLOBAL_ALL);
}

SongLoader::~SongLoader() {
    curl_global_cleanup();
}

void SongLoader::download(const std::shared_ptr<Song>& song) {
    std::unique_lock lock(mutex);
    songs.emplace(song->getId(), song);
    downloadQueue.push(song->getId());
}

void SongLoader::downloadLoop(){
    std::ofstream file = std::ofstream(directory / "songs.csv", std::ios::out | std::ios::app);
    while(true) {
        std::unique_lock lock(mutex);
        if(downloadQueue.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        QString id = downloadQueue.front();
        downloadQueue.pop();
        lock.unlock();
        std::filesystem::path path = directory / (id.toStdString() + ".wav");
        CURL* curl = curl_easy_init();
        QImage image;
        //ACTUALLY DOWNLOAD IMAGE AND SAVE IT TO THE FILE HERE
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
        system(("youtube-dl -f bestaudio --extract-audio --audio-format wav --output \"" + path.parent_path().string() + (const char)std::filesystem::path::preferred_separator + id.toStdString() + ".%(ext)s\" " + id.toStdString()).c_str());



        lock.lock();
        if(!file.is_open())
            throw std::runtime_error("Could not open songs.csv");
        file << (id.toStdString() + ',' + songs.at(id)->getName().toStdString() + ',' + songs.at(id)->getAuthor().toStdString()) << std::endl;
        songs.at(id)->setDownloaded();
        lock.unlock();

    }
}

std::shared_ptr<Song> SongLoader::getRand() {
    std::shared_lock lock(mutex);
    if(songs.empty()) {
        return nullptr;
    }
    int rand = std::rand() % songs.size();
    auto it = songs.begin();
    std::advance(it, rand);
    return it->second;
}

std::shared_ptr<Song> SongLoader::getById(const QString &id){
    std::shared_lock lock(mutex);
    return songs.at(id);
}

bool SongLoader::isDownloaded(const QString &id) {
    std::shared_lock lock(mutex);
    return songs.contains(id);
}

QString SongLoader::getSongPath(const QString &id) {
    return QString::fromStdString((directory / (id.toStdString() + ".wav")).string());
}
