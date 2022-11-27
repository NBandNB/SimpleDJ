#pragma once

#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "SongLoader.h"

class UnlockPopup : public QDialog
{
    Q_OBJECT

public:
    explicit UnlockPopup(std::shared_ptr<SongLoader> songLoader, QWidget *parent = Q_NULLPTR);
    ~UnlockPopup() = default;

private:
    std::shared_ptr<SongLoader> songLoader;

    std::shared_ptr<QVBoxLayout> layout;
    std::shared_ptr<QLineEdit> pinBox;
    std::shared_ptr<QHBoxLayout> confirmLayout;
    std::shared_ptr<QPushButton> confirmButton;
    std::shared_ptr<QPushButton> cancelButton;

    void confirm();
    void close();
};
