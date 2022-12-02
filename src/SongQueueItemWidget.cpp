#include <QLabel>
#include "SongQueueItemWidget.h"

SongQueueItemWidget::SongQueueItemWidget(const std::shared_ptr<Song>& song, QWidget *parent)
    : QWidget(parent),
    song(song),
    layout(std::make_shared<QGridLayout>(this)),
    songNameLabel(std::make_shared<QLabel>(song->getName(), this)),
    artistNameLabel(std::make_shared<QLabel>(song->getAuthor(), this)),
    imageLabel(std::make_shared<QLabel>(this)),
    removeButton(std::make_shared<QPushButton>("Remove", this))
{
    if(!song->getImageDownloaded()){
        connect(song.get(), &Song::imageDownloadedSignal, this, &SongQueueItemWidget::loadImage);
    }
    else
        imageLabel->setPixmap(QPixmap::fromImage(song->getImage()).scaled(200, 200, Qt::KeepAspectRatio));
    layout->addWidget(imageLabel.get(), 0, 0, 3, 1);
    layout->addWidget(songNameLabel.get(), 0, 1);
    layout->addWidget(artistNameLabel.get(), 1, 1);
    layout->addWidget(removeButton.get(), 2, 1);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 2, 3, 1);
    setLayout(layout.get());
    show();
}

std::shared_ptr<QPushButton> SongQueueItemWidget::getRemoveButton() const {
    return removeButton;
}

std::shared_ptr<Song> SongQueueItemWidget::getSong() const {
    return song;
}

void SongQueueItemWidget::loadImage() {
    imageLabel->setPixmap(QPixmap::fromImage(song->getImage()).scaled(200, 200, Qt::KeepAspectRatio));
}
