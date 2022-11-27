#include <QVBoxLayout>
#include <utility>
#include "SongQueueWidget.h"
#include <thread>

SongQueueWidget::SongQueueWidget(std::shared_ptr<SongLoader> songLoader, QWidget *parent)
    : QScrollArea(parent),
    songLoader(std::move(songLoader)),
    layout(std::make_shared<QVBoxLayout>(this)),
    defaultQueueLayout(std::make_shared<QueueLayout>(songLoader, this)),
    requestedQueueLayout(std::make_shared<QueueLayout>(songLoader, this)),
    defaultQueueLabel(std::make_shared<QLabel>("Default queue", this)),
    requestedQueueLabel(std::make_shared<QLabel>("Requested queue", this)),
    mainWidget(new QWidget(this))
{
    layout->addWidget(requestedQueueLabel.get());
    layout->addLayout(requestedQueueLayout.get());
    layout->addWidget(defaultQueueLabel.get());
    if(this->songLoader->getRand())
    {
        for(int i = 0; i < 20; i++)
            defaultQueueLayout->addSong(this->songLoader->getRand());
    }
    layout->addLayout(defaultQueueLayout.get());
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    QObject::connect(requestedQueueLayout.get(), &QueueLayout::updatedSignal, this, &SongQueueWidget::requestedQueueUpdated);
    QObject::connect(defaultQueueLayout.get(), &QueueLayout::updatedSignal, this, &SongQueueWidget::defaultQueueUpdated);
    mainWidget->setLayout(layout.get());
    setWidget(mainWidget);
    show();
}

std::shared_ptr<QueueLayout> SongQueueWidget::getDefaultQueue() {
    return defaultQueueLayout;
}

std::shared_ptr<QueueLayout> SongQueueWidget::getRequestedQueue() {
    return requestedQueueLayout;
}

void SongQueueWidget::requestedQueueUpdated() {
    topOffQueue();
    defaultQueueLayout->update();
    mainWidget->adjustSize();
    adjustSize();
}

void SongQueueWidget::defaultQueueUpdated() {
    topOffQueue();
    requestedQueueLayout->update();
    mainWidget->adjustSize();
    adjustSize();
}

void SongQueueWidget::topOffQueue() {
    if(songLoader->getRand())
        for(int i = requestedQueueLayout->count() + defaultQueueLayout->count(); i < 20; i++)
            defaultQueueLayout->addSong(this->songLoader->getRand(), true);
}
