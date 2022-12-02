#include "MainWindow.h"
#include <thread>

MainWindow::MainWindow(const std::filesystem::path& appDir, QWidget *parent)
    : QMainWindow(parent),
    songLoader(std::make_shared<SongLoader>(appDir)),
    centralWidget(std::make_shared<QWidget>(this)),
    songQueueWidget(std::make_shared<SongQueueWidget>(songLoader, centralWidget.get())),
    songSearchWidget(std::make_shared<SongSearchWidget>(songQueueWidget->getRequestedQueue(), songLoader, centralWidget.get())),
    currentSongWidget(std::make_shared<CurrentSongWidget>(songQueueWidget->getRequestedQueue(), songQueueWidget->getDefaultQueue(), songLoader, centralWidget.get())),
    lowerLayout(std::make_shared<QHBoxLayout>()),
    mainLayout(std::make_shared<QVBoxLayout>(centralWidget.get()))
{
    setWindowTitle("SimpleDJ");
    setMinimumSize(1000, 600);
    setCentralWidget(centralWidget.get());
    mainLayout->addWidget(currentSongWidget.get());
    lowerLayout->addWidget(songQueueWidget.get());
    lowerLayout->addWidget(songSearchWidget.get());
    mainLayout->addLayout(lowerLayout.get());
}