#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <memory>
#include "Song.h"
#include "SongLoader.h"
#include "QueueLayout.h"

class SongSearchItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SongSearchItemWidget(const std::shared_ptr<Song>& song, std::shared_ptr<SongLoader> songLoader, std::shared_ptr<QueueLayout> requestedQueue, QWidget *parent = Q_NULLPTR);

    void adjustSize();
private:
    std::shared_ptr<QVBoxLayout> layout;
    std::shared_ptr<Song> song;

    std::shared_ptr<QueueLayout> requestedQueue;
    std::shared_ptr<SongLoader> songLoader;

    std::shared_ptr<QLabel> songNameLabel;
    std::shared_ptr<QLabel> authorNameLabel;

    std::shared_ptr<QPushButton> addButton;
    std::shared_ptr<QPushButton> deleteButton;
    std::shared_ptr<QPushButton> downloadButton;
    std::shared_ptr<QPushButton> downloadAndAddButton;

    void addToQueue();
    void deleteSong();
    void download();
    void downloadAndAddToQueue();

    void updateButtons();

private slots:
    void downloaded(const QString& id);
    void deleted(const QString& id);
    void unlocked();
    void locked();

};
