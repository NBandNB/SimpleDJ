#include "SongSearchItemWidget.h"

SongSearchItemWidget::SongSearchItemWidget(const std::shared_ptr<Song>& song, std::shared_ptr<SongLoader> songLoader, std::shared_ptr<QueueLayout> requestedQueue, QWidget *parent)
    : QWidget(parent),
    song(song),
    requestedQueue(std::move(requestedQueue)),
    songLoader(std::move(songLoader)),
    layout(std::make_shared<QVBoxLayout>(this)),
    songNameLabel(std::make_shared<QLabel>(song->getName(), this)),
    authorNameLabel(std::make_shared<QLabel>(song->getAuthor(), this)),
    addButton(std::make_shared<QPushButton>("Add to Queue", this)),
    downloadButton(std::make_shared<QPushButton>("Download", this)),
    downloadAndAddButton(std::make_shared<QPushButton>("Download and Add to Queue", this))
{
    layout->addWidget(songNameLabel.get());
    layout->addWidget(authorNameLabel.get());
    layout->addWidget(addButton.get());
    layout->addWidget(downloadButton.get());
    layout->addWidget(downloadAndAddButton.get());
    connect(addButton.get(), &QAbstractButton::clicked, this, &SongSearchItemWidget::addToQueue);
    connect(downloadButton.get(), &QAbstractButton::clicked, this, &SongSearchItemWidget::download);
    connect(downloadAndAddButton.get(), &QAbstractButton::clicked, this, &SongSearchItemWidget::downloadAndAddToQueue);
    updateButtons();
    setLayout(layout.get());
    connect(this->songLoader.get(), &SongLoader::downloadCompleteSignal, this, &SongSearchItemWidget::downloaded);
    connect(this->songLoader.get(), &SongLoader::unlockedSignal, this, &SongSearchItemWidget::unlocked);
    connect(this->songLoader.get(), &SongLoader::lockedSignal, this, &SongSearchItemWidget::locked);
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

void SongSearchItemWidget::locked() {
    updateButtons();
}

void SongSearchItemWidget::unlocked(){
    updateButtons();
}

void SongSearchItemWidget::downloaded(const QString& id){
    if(id == song->getId()){
        updateButtons();
    }
}

void SongSearchItemWidget::updateButtons() {
    if(song->getDownloaded()){
        addButton->show();
        downloadButton->hide();
        downloadAndAddButton->hide();
    } else if(!this->songLoader->isLocked()) {
        addButton->hide();
        downloadButton->show();
        downloadAndAddButton->show();
    } else {
        addButton->hide();
        downloadButton->hide();
        downloadAndAddButton->hide();
    }
}