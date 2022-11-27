#include "UnlockPopup.h"

UnlockPopup::UnlockPopup(std::shared_ptr<SongLoader> songLoader, QWidget *parent)
    : QDialog(parent),
    layout(std::make_shared<QVBoxLayout>(this)),
    pinBox(std::make_shared<QLineEdit>(this)),
    confirmLayout(std::make_shared<QHBoxLayout>(this)),
    confirmButton(std::make_shared<QPushButton>("Confirm", this)),
    cancelButton(std::make_shared<QPushButton>("Cancel", this)),
    songLoader(std::move(songLoader))
{
    pinBox->setEchoMode(QLineEdit::Password);
    layout->addWidget(pinBox.get());
    layout->addLayout(confirmLayout.get());
    confirmLayout->addWidget(confirmButton.get());
    confirmLayout->addWidget(cancelButton.get());

    connect(confirmButton.get(), &QPushButton::clicked, this, &UnlockPopup::confirm);
    connect(cancelButton.get(), &QPushButton::clicked, this, &UnlockPopup::close);
}

void UnlockPopup::confirm()
{
    if(!songLoader->unlock(pinBox->text()))
        close();
    else
        pinBox->clear();
}

void UnlockPopup::close()
{
    pinBox->clear();
    QDialog::close();
}