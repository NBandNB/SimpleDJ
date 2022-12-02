#include <QueueLayout.h>
#include <iostream>
#include "SongQueueItemWidget.h"

QueueLayout::QueueLayout(std::shared_ptr<SongLoader> songLoader, QWidget *parent)
    : QVBoxLayout(parent),
    songLoader(std::move(songLoader))
{
    connect(this->songLoader.get(), &SongLoader::deleteSongSignal, this, &QueueLayout::deleteSong);
}

//Add a song to the end of the queue.
void QueueLayout::addSong(const std::shared_ptr<Song>& song, bool noSignal)
{
    addWidget(new SongQueueItemWidget(song, this->parentWidget()));
    std::cout << "Added song " << song->getName().toStdString() << " to queue, " << count() << "songs in queue" << std::endl;
    connect(dynamic_cast<SongQueueItemWidget*>(itemAt(count() - 1)->widget())->getRemoveButton().get(), &QPushButton::clicked, this, &QueueLayout::removeSong);
    update();
    //If we have been asked to not emit a signal, don't emit a signal.
    //This should only happen if we are adding songs during a queue update.
    if(!noSignal){
        updatedSignal();
    }
}

//Removes the song that owns the button from which this slot was called.
void QueueLayout::removeSong()
{
    removeWidget(dynamic_cast<QPushButton*>(sender())->parentWidget());
    dynamic_cast<QPushButton*>(sender())->parentWidget()->deleteLater();
    updatedSignal();
}

//Deletes all widgets that are related to the song that was deleted.
void QueueLayout::deleteSong(const QString &id) {
    std::vector<int> toRemove;
    for(int i = 0; i < count(); i++){
        if(dynamic_cast<SongQueueItemWidget*>(itemAt(i)->widget())->getSong()->getId() == id){
            toRemove.push_back(i);
        }
    }
    int offset = 0;
    for(int i : toRemove){
        auto* widget = itemAt(i - offset)->widget();
        removeWidget(itemAt(i - offset)->widget());
        widget->deleteLater();
        offset += 1;
    }
    updatedSignal();
}

//Returns number of songs in queue.
int32_t QueueLayout::count() const {
    return QBoxLayout::count();
}

//Gets the next downloaded song in queue, returns a pointer to it and deletes the widget.
std::shared_ptr<Song> QueueLayout::getNextSong() {
    std::shared_ptr<Song> song;
    for(int i = 0; i < count(); i++){
        auto* songQueueItemWidget = dynamic_cast<SongQueueItemWidget*>(itemAt(i)->widget());
        if(songQueueItemWidget->getSong()->getDownloaded()){
            song = songQueueItemWidget->getSong();
            QBoxLayout::takeAt(i)->widget()->deleteLater();
            updatedSignal();
            return song;
        }
    }
    return song;
}

//Returns true if there are any songs(that have been downloaded) in the queue.
bool QueueLayout::hasSongs() const {
    if(count() > 0){
        for(int i = 0; i < count(); i++)
            if(dynamic_cast<SongQueueItemWidget*>(itemAt(i)->widget())->getSong()->getDownloaded())
                return true;
    }
    return false;
}

