#pragma once

#include <QVBoxLayout>
#include <Song.h>

class QueueLayout : public QVBoxLayout
{
    Q_OBJECT

public:
    explicit QueueLayout(QWidget *parent = Q_NULLPTR);

    void addSong(const std::shared_ptr<Song>& song, bool noSignal = false);

    int32_t count() const override;

    std::shared_ptr<Song> getNextSong();

private slots:
    void removeSong();

signals:
    void updatedSignal();
};