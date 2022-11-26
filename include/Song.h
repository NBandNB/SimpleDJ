#pragma once

#include <string>
#include <QImage>
#include <QVariant>


class Song : public QObject
{
    Q_OBJECT
private:
    QString name;
    QString author;
    QImage image;
    QString id;
    bool downloaded = false;
    bool imageDownloaded = false;

public:
    Song();
    Song(QString name, QString author, QImage image, QString id, bool downloaded = false);

    QString getName();
    QString getAuthor();
    QImage getImage();
    QString getId();
    bool getDownloaded();
    bool getImageDownloaded();

    void setName(QString name);
    void setAuthor(QString author);
    void setImage(QImage image);
    void setDownloaded();
    void setImageDownloaded();

signals:
    void downloadedSignal();
    void imageDownloadedSignal();
};
Q_DECLARE_METATYPE(Song)