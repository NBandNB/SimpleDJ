#pragma once

#include <QtWidgets/QDockWidget>
#include <QLineEdit>
#include <QScrollArea>
#include <QBoxLayout>
#include <QPushButton>
#include "Song.h"
#include "SongLoader.h"
#include "QueueLayout.h"

class SongSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SongSearchWidget(std::shared_ptr<QueueLayout> requestedQueue, std::shared_ptr<SongLoader> songLoader, QWidget *parent = Q_NULLPTR);
    ~SongSearchWidget() override;


private slots:
    void search();
    void downloadComplete(const std::vector<std::shared_ptr<Song>>& song);

private:
    void downloadLoop();

    std::shared_mutex searchTermMutex;
    std::string searchTerm;

    std::shared_ptr<QVBoxLayout> layout;
    std::shared_ptr<QHBoxLayout> searchLayout;
    std::shared_ptr<QLineEdit> searchBox;
    std::shared_ptr<QPushButton> searchButton;
    std::shared_ptr<QScrollArea> searchResults;
    std::shared_ptr<QWidget> searchResultsWidget;
    std::shared_ptr<QVBoxLayout> searchResultsLayout;

    std::shared_ptr<QueueLayout> requestedQueue{};

    std::shared_ptr<SongLoader> songLoader;

signals:
    void downloadCompleteSignal(const std::vector<std::shared_ptr<Song>>& song);
};
