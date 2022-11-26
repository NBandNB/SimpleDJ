#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QBoxLayout>
#include "Song.h"
#include "SongLoader.h"
#include "QueueLayout.h"

class SongSearchItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SongSearchItemWidget(std::shared_ptr<Song> song, std::shared_ptr<SongLoader> songLoader, std::shared_ptr<QueueLayout> requestedQueue, QWidget *parent = Q_NULLPTR);

    void adjustSize();
private:
    std::shared_ptr<QVBoxLayout> layout;
    std::shared_ptr<Song> song;

    std::shared_ptr<QueueLayout> requestedQueue;
    std::shared_ptr<SongLoader> songLoader;

    void addToQueue();
    void download();
    void downloadAndAddToQueue();
};
