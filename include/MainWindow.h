#pragma once

#include <QtWidgets/QMainWindow>
#include <list>
#include <QVBoxLayout>
#include <memory>
#include "CurrentSongWidget.h"
#include "SongQueueWidget.h"
#include "SongSearchWidget.h"
#include "SongLoader.h"

#include "Song.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::filesystem::path& appDir, QWidget *parent = Q_NULLPTR);
private:
    std::shared_ptr<SongLoader> songLoader;

    std::shared_ptr<QWidget> centralWidget;
    std::shared_ptr<QVBoxLayout> mainLayout;

    std::shared_ptr<QHBoxLayout> lowerLayout;
    std::shared_ptr<SongQueueWidget> songQueueWidget;
    std::shared_ptr<SongSearchWidget> songSearchWidget;
    std::shared_ptr<CurrentSongWidget> currentSongWidget;
};