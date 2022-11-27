#include "ChangePinPopup.h"

ChangePinPopup::ChangePinPopup(std::shared_ptr<SongLoader> songLoader, QWidget *parent)
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

    connect(confirmButton.get(), &QPushButton::clicked, this, &ChangePinPopup::confirm);
    connect(cancelButton.get(), &QPushButton::clicked, this, &ChangePinPopup::close);
}

void ChangePinPopup::confirm()
{
    if(!songLoader->isLocked())
        songLoader->setPin(pinBox->text());
    close();
}

void ChangePinPopup::close() {
    pinBox->clear();
    QDialog::close();
}