#pragma once

#include <QtWidgets/QDockWidget>
#include <QListView>
#include <QScrollArea>
#include <QLabel>
#include "Song.h"
#include <list>
#include "SongLoader.h"
#include "QueueLayout.h"

class SongQueueWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit SongQueueWidget(std::shared_ptr<SongLoader> songLoader, QWidget *parent = Q_NULLPTR);
    ~SongQueueWidget() override = default;

    std::shared_ptr<QueueLayout> getDefaultQueue();
    std::shared_ptr<QueueLayout> getRequestedQueue();

private:
    std::shared_ptr<QVBoxLayout> layout;
    std::shared_ptr<QueueLayout> defaultQueueLayout;
    std::shared_ptr<QueueLayout> requestedQueueLayout;

    std::shared_ptr<QLabel> defaultQueueLabel;
    std::shared_ptr<QLabel> requestedQueueLabel;

    std::shared_ptr<QWidget> mainWidget;

    std::shared_ptr<SongLoader> songLoader;

    void topOffQueue();

private slots:
    void requestedQueueUpdated();
    void defaultQueueUpdated();
};
