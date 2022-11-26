#include "CurrentSongWidget.h"
#include <QAudioOutput>

CurrentSongWidget::CurrentSongWidget(std::shared_ptr<QueueLayout> requestedQueue, std::shared_ptr<QueueLayout> defaultQueue, std::shared_ptr<SongLoader> songLoader, QWidget *parent)
    : QWidget(parent),
    layout(std::make_shared<QGridLayout>(this)),
    image(std::make_shared<QLabel>(this)),
    title(std::make_shared<QLabel>(this)),
    author(std::make_shared<QLabel>(this)),
    timeLabel(std::make_shared<QLabel>(this)),
    timeLabel2(std::make_shared<QLabel>(this)),
    pauseAtEndButtonButton(std::make_shared<QPushButton>("Pause at end", this)),
    playNextSongButton(std::make_shared<QPushButton>("Play next song", this)),
    playButton(std::make_shared<QPushButton>("Play", this)),
    pauseButton(std::make_shared<QPushButton>("Pause", this)),
    returnToBeginningButton(std::make_shared<QPushButton>("Return to beginning", this)),
    requestedQueue(std::move(requestedQueue)),
    defaultQueue(std::move(defaultQueue)),
    player(std::make_shared<QMediaPlayer>(this)),
    audioOutput(std::make_shared<QAudioOutput>()),
    songLoader(std::move(songLoader))
{
    player->setAudioOutput(audioOutput.get());
    setLayout(layout.get());
    layout->addWidget(image.get(), 0, 0, 3, 1);
    layout->addWidget(title.get(), 0, 1, 1, 2);
    layout->addWidget(author.get(), 1, 1, 1, 2);
    layout->addWidget(timeLabel.get(), 2, 1);
    layout->addWidget(timeLabel2.get(), 2, 2);
    layout->addWidget(pauseAtEndButtonButton.get(), 2, 3, 1, 2);
    layout->addWidget(playNextSongButton.get(), 0, 3);
    layout->addWidget(returnToBeginningButton.get(), 0, 4);
    layout->addWidget(playButton.get(), 1, 3);
    layout->addWidget(pauseButton.get(), 1, 4);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 5, 3, 1);
    setSong(this->defaultQueue->getNextSong());
    connect(player.get(), &QMediaPlayer::mediaStatusChanged, this, &CurrentSongWidget::mediaStatusChanged);
    connect(player.get(), &QMediaPlayer::positionChanged, this, &CurrentSongWidget::positionChanged);
    connect(pauseAtEndButtonButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::pauseAtEndButton);
    connect(playNextSongButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::playNextSong);
    connect(playButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::play);
    connect(pauseButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::pause);
    connect(returnToBeginningButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::returnToBeginning);
    show();
}

void CurrentSongWidget::setSong(const std::shared_ptr<Song>& song) {
    currentSong = song;
    image->setPixmap(QPixmap::fromImage(song->getImage()).scaled(250, 250, Qt::KeepAspectRatio));
    title->setText(song->getName());
    author->setText(song->getAuthor());
    player->setSource(QUrl::fromLocalFile(songLoader->getSongPath(song->getId())));
    player->play();
    adjustSize();
}

void CurrentSongWidget::mediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        playNextSong();
        if(pauseAtEnd){
            pause();
        }
    }
    else if (status == QMediaPlayer::LoadedMedia) {
        timeLabel2->setText(QString::number(player->duration() / 1000));
    }
}

void CurrentSongWidget::positionChanged(qint64 position) {
    timeLabel->setText(QString::number(position / 1000));
}

void CurrentSongWidget::pauseAtEndButton() {
    pauseAtEnd = true;
}

void CurrentSongWidget::playNextSong() {
    if(requestedQueue->hasSongs()){
        setSong(requestedQueue->getNextSong());
    } else if(defaultQueue->hasSongs()){
        setSong(defaultQueue->getNextSong());
    } else {
        std::shared_ptr<Song> song;
        for (int i = 0; i < 10; i++) {
            song = songLoader->getRand();
            if (song->getDownloaded()) {
                break;
            }
        }
        if (!song->getDownloaded()) {
            player->pause();
        }
        setSong(song);
    }
}

void CurrentSongWidget::play() {
    player->play();
}

void CurrentSongWidget::pause() {
    player->pause();
}

void CurrentSongWidget::returnToBeginning() {
    player->setPosition(0);
}

