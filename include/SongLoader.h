#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <queue>
#include <fstream>
#include "Song.h"

class SongLoader{
private:
    std::filesystem::path directory;

    std::shared_mutex mutex;
    std::unordered_map<QString, std::shared_ptr<Song>> songs;
    std::queue<QString> downloadQueue;

public:
    SongLoader(const std::filesystem::path& directory);
    ~SongLoader();

    void download(const std::shared_ptr<Song>& song);
    void downloadLoop();
    std::shared_ptr<Song> getRand();
    std::shared_ptr<Song> getById(const QString& id);
    bool isDownloaded(const QString& id);
    QString getSongPath(const QString& id);
};