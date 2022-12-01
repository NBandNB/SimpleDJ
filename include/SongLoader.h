#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <queue>
#include <fstream>
#include "Song.h"

class SongLoader : public QObject
{
    Q_OBJECT
private:
    std::filesystem::path directory;

    std::shared_mutex mutex;
    std::unordered_map<QString, std::shared_ptr<Song>> songs;
    std::queue<std::pair<QString, bool>> downloadQueue;
    std::queue<QString> deleteQueue;
    std::list<std::string> songIds;

    bool locked;
    QString pin;

public:
    SongLoader(const std::filesystem::path& directory);
    ~SongLoader();

    void download(const std::shared_ptr<Song>& song);
    void downloadLoop();
    std::shared_ptr<Song> getRand();
    std::shared_ptr<Song> getById(const QString& id);
    bool isDownloaded(const QString& id);
    bool containsSong(const QString& id);
    QString getSongPath(const QString& id);
    void deleteSong(const QString& id);
    bool hasDownloaded();

    bool isLocked() const;
    void setPin(const QString& newPin);
    bool unlock(const QString& pinAttempt);
    void lock();
signals:
    void lockedSignal();
    void unlockedSignal();
    void downloadCompleteSignal(const QString& id);
    void deleteSongSignal(const QString& id);

};