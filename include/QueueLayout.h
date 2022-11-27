#pragma once

#include <QVBoxLayout>
#include "Song.h"
#include "SongLoader.h"

class QueueLayout : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit QueueLayout(std::shared_ptr<SongLoader> songLoader, QWidget *parent = Q_NULLPTR);

    void addSong(const std::shared_ptr<Song>& song, bool noSignal = false);

    int32_t count() const override;

    std::shared_ptr<Song> getNextSong();
    bool hasSongs() const;

private slots:
    void removeSong();
    void deleteSong(const QString& id);
signals:
    void updatedSignal();
private:
    std::shared_ptr<SongLoader> songLoader;
};