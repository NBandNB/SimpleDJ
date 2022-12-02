#include "Song.h"

#include <utility>

Song::Song() = default;

Song::Song(QString name, QString author, QImage image, QString id, bool downloaded, bool imageDownloaded)
    : name(std::move(name)), author(std::move(author)), image(std::move(image)), id(std::move(id)), downloaded(downloaded), imageDownloaded(imageDownloaded) {
}

QString Song::getName() {
    return name;
}

QString Song::getAuthor() {
    return author;
}

QImage Song::getImage() {
    return image;
}

QString Song::getId() {
    return id;
}

bool Song::getDownloaded() const {
    return downloaded;
}

bool Song::getImageDownloaded() const {
    return imageDownloaded;
}

void Song::setName(QString name) {
    this->name = std::move(name);
}

void Song::setAuthor(QString author) {
    this->author = std::move(author);
}

void Song::setImage(QImage image) {
    this->image = std::move(image);
}

void Song::setDownloaded(bool newDownloaded) {
    this->downloaded = newDownloaded;
    downloadedSignal();
}

void Song::setImageDownloaded(bool newDownloaded) {
    imageDownloaded = newDownloaded;
    imageDownloadedSignal();
}