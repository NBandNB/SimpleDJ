#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QBoxLayout>
#include "Song.h"
#include "SongQueueWidget.h"
#include <memory>
#include <QLabel>
#include <QPushButton>

class SongQueueItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SongQueueItemWidget(const std::shared_ptr<Song>& song, QWidget *parent = Q_NULLPTR);

    std::shared_ptr<QPushButton> getRemoveButton() const;

    std::shared_ptr<Song> getSong() const;
private slots:
    void loadImage();

private:
    std::shared_ptr<Song> song;

    std::shared_ptr<QLabel> songNameLabel;
    std::shared_ptr<QLabel> artistNameLabel;
    std::shared_ptr<QLabel> imageLabel;

    std::shared_ptr<QPushButton> removeButton;

    std::shared_ptr<QGridLayout> layout;
};
