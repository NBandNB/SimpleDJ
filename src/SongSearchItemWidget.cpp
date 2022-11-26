#include <QLabel>
#include <utility>
#include <QAbstractButton>
#include <QPushButton>
#include "SongSearchItemWidget.h"

SongSearchItemWidget::SongSearchItemWidget(std::shared_ptr<Song> song, std::shared_ptr<SongLoader> songLoader, std::shared_ptr<QueueLayout> requestedQueue, QWidget *parent)
    : QWidget(parent),
    song(song),
    requestedQueue(std::move(requestedQueue)),
    songLoader(std::move(songLoader)),
    layout(std::make_shared<QVBoxLayout>(this))
{
    layout->addWidget(new QLabel(song->getName(), this));
    layout->addWidget(new QLabel(song->getAuthor(), this));
    if(song->getDownloaded()){
        auto* addToQueueButton = new QPushButton("Add to queue", this);
        connect(addToQueueButton, &QAbstractButton::clicked, this, &SongSearchItemWidget::addToQueue);
        layout->addWidget(addToQueueButton);
    } else {
        auto* downloadButton = new QPushButton("Download", this);
        connect(downloadButton, &QAbstractButton::clicked, this, &SongSearchItemWidget::download);
        layout->addWidget(downloadButton);
        auto* downloadAndAddToQueueButton = new QPushButton("Download and add to queue", this);
        connect(downloadAndAddToQueueButton, &QAbstractButton::clicked, this, &SongSearchItemWidget::downloadAndAddToQueue);
        layout->addWidget(downloadAndAddToQueueButton);
    }
    setLayout(layout.get());
    show();
}

void SongSearchItemWidget::addToQueue(){
    requestedQueue->addSong(songLoader->getById(song->getId()));
}

void SongSearchItemWidget::download(){
    songLoader->download(song);
}

void SongSearchItemWidget::downloadAndAddToQueue(){
    songLoader->download(song);
    requestedQueue->addSong(songLoader->getById(song->getId()));
}

void SongSearchItemWidget::adjustSize() {
    setMaximumWidth(parentWidget()->width());
    QWidget::adjustSize();
}