#pragma once

#include <QtWidgets/QWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include "Song.h"
#include "QueueLayout.h"
#include "SongLoader.h"

class CurrentSongWidget : public QWidget
{
    Q_OBJECT
private:
    std::shared_ptr<Song> currentSong;

public:
    explicit CurrentSongWidget(std::shared_ptr<QueueLayout> requestedQueue, std::shared_ptr<QueueLayout> defaultQueue, std::shared_ptr<SongLoader> songLoader, QWidget *parent = Q_NULLPTR);

private slots:
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void pauseAtEndButton();
    void playNextSong();
    void play();
    void pause();
    void returnToBeginning();
    void changePin();
    void lock();
    void unlock();
    void unlockButtonPress();
    void lockButtonPress();
    void songDeleted(const QString& id);
    void songDownloaded(const QString& id);


private:
    bool pauseAtEnd = false;

    void setSong(const std::shared_ptr<Song>& song);

    std::shared_ptr<QGridLayout> layout;

    std::shared_ptr<QLabel> image;
    std::shared_ptr<QLabel> title;
    std::shared_ptr<QLabel> author;

    std::shared_ptr<QLabel> timeLabel;
    std::shared_ptr<QLabel> timeLabel2;

    std::shared_ptr<QPushButton> pauseAtEndButtonButton;
    std::shared_ptr<QPushButton> playNextSongButton;
    std::shared_ptr<QPushButton> playButton;
    std::shared_ptr<QPushButton> pauseButton;
    std::shared_ptr<QPushButton> returnToBeginningButton;

    std::shared_ptr<QPushButton> lockButton;
    std::shared_ptr<QPushButton> unlockButton;
    std::shared_ptr<QPushButton> changePinButton;

    std::shared_ptr<QueueLayout> requestedQueue;
    std::shared_ptr<QueueLayout> defaultQueue;
    std::shared_ptr<SongLoader> songLoader;

    std::shared_ptr<QAudioOutput> audioOutput;
    std::shared_ptr<QMediaPlayer> player;
};
