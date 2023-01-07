#include "CurrentSongWidget.h"
#include <QAudioOutput>
#include "UnlockPopup.h"
#include "ChangePinPopup.h"

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
    lockButton(std::make_shared<QPushButton>("Lock", this)),
    unlockButton(std::make_shared<QPushButton>("Unlock", this)),
    changePinButton(std::make_shared<QPushButton>("Change pin", this)),
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
    layout->addWidget(lockButton.get(), 0, 5);
    layout->addWidget(unlockButton.get(), 1, 5);
    layout->addWidget(changePinButton.get(), 2, 5);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 6, 3, 1);
    if(this->defaultQueue->hasSongs())
    {
        setSong(this->defaultQueue->getNextSong());
    } else
    {
        timeLabel->setText("00:00");
        timeLabel2->setText("00:00");
    }
    connect(player.get(), &QMediaPlayer::mediaStatusChanged, this, &CurrentSongWidget::mediaStatusChanged);
    connect(player.get(), &QMediaPlayer::positionChanged, this, &CurrentSongWidget::positionChanged);
    connect(player.get(), &QMediaPlayer::durationChanged, this, &CurrentSongWidget::durationChanged);
    connect(pauseAtEndButtonButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::pauseAtEndButton);
    connect(playNextSongButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::playNextSong);
    connect(playButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::play);
    connect(pauseButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::pause);
    connect(returnToBeginningButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::returnToBeginning);
    connect(lockButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::lockButtonPress);
    connect(unlockButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::unlockButtonPress);
    connect(changePinButton.get(), &QPushButton::clicked, this, &CurrentSongWidget::changePin);
    connect(this->songLoader.get(), &SongLoader::lockedSignal, this, &CurrentSongWidget::lock);
    connect(this->songLoader.get(), &SongLoader::unlockedSignal, this, &CurrentSongWidget::unlock);
    connect(this->songLoader.get(), &SongLoader::deleteSongSignal, this, &CurrentSongWidget::songDeleted);
    connect(this->songLoader.get(), &SongLoader::downloadCompleteSignal, this, &CurrentSongWidget::songDownloaded);
    unlock();
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

QString toTime(long long time) {
    QString hours = QString::number(time / 3600);
    QString minutes = QString::number((time / 60) % 60);
    QString seconds = QString::number(time % 60);
    if(seconds.length() == 1) {
        seconds = "0" + seconds;
    }
    if(minutes.length() == 1) {
        minutes = "0" + minutes;
    }
    if(hours == "0"){
        return minutes + ":" + seconds;
    }
    return hours + ":" + minutes + ":" + seconds;
}

void CurrentSongWidget::mediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        playNextSong();
        if(pauseAtEnd){
            pauseAtEnd = false;
            pause();
        }
    }
}

void CurrentSongWidget::positionChanged(qint64 position) {
    timeLabel->setText(toTime(position / 1000));
}

void CurrentSongWidget::durationChanged(qint64 duration) {
    timeLabel2->setText(toTime(duration / 1000));
}

void CurrentSongWidget::pauseAtEndButton() {
    pauseAtEnd = true;
}

void CurrentSongWidget::playNextSong() {
    if(requestedQueue->hasSongs()){
        setSong(requestedQueue->getNextSong());
    } else if(defaultQueue->hasSongs()){
        setSong(defaultQueue->getNextSong());
    } else if(songLoader->hasDownloaded()){
        setSong(songLoader->getRand());
    } else {
        player->stop();
        currentSong = nullptr;
        image->setPixmap(QPixmap::fromImage(QImage()).scaled(250, 250, Qt::KeepAspectRatio));
        title->setText("");
        author->setText("");
        player->setSource(QUrl::fromLocalFile(""));
        player->play();
        adjustSize();
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

void CurrentSongWidget::changePin() {
    ChangePinPopup popup(songLoader, this);
    popup.exec();
}

void CurrentSongWidget::lockButtonPress() {
    songLoader->lock();
}

void CurrentSongWidget::unlockButtonPress() {
    UnlockPopup popup(songLoader, this);
    popup.exec();
}

void CurrentSongWidget::lock() {
    lockButton->setEnabled(false);
    unlockButton->setEnabled(true);
    changePinButton->setEnabled(false);
}

void CurrentSongWidget::unlock() {
    lockButton->setEnabled(true);
    unlockButton->setEnabled(false);
    changePinButton->setEnabled(true);
}

void CurrentSongWidget::songDeleted(const QString& id) {
    if(id == currentSong->getId()){
        playNextSong();
    }
}

void CurrentSongWidget::songDownloaded(const QString& id) {
    if(player->playbackState() == QMediaPlayer::StoppedState){
        playNextSong();
    }
}

