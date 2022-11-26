#include "SongSearchWidget.h"
#include "SongSearchItemWidget.h"

#include <utility>
#include <curl/curl.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <iostream>
#include <QLabel>
#include <thread>

SongSearchWidget::SongSearchWidget(std::shared_ptr<QueueLayout> requestedQueue, std::shared_ptr<SongLoader> songLoader, QWidget *parent)
    : QWidget(parent),
    songLoader(std::move(songLoader)),
    requestedQueue(std::move(requestedQueue)),
    searchLayout(std::make_shared<QHBoxLayout>(this)),
    searchButton(std::make_shared<QPushButton>(this)),
    searchBox(std::make_shared<QLineEdit>(this)),
    searchResults(std::make_shared<QScrollArea>(this)),
    searchResultsWidget(std::make_shared<QWidget>(searchResults.get())),
    searchResultsLayout(std::make_shared<QVBoxLayout>(searchResultsWidget.get())),
    layout(std::make_shared<QVBoxLayout>(this))
{
    setMinimumWidth(300);
    searchButton->setText("Search");
    searchLayout->addWidget(searchBox.get());
    searchLayout->addWidget(searchButton.get());
    layout->addLayout(searchLayout.get());
    searchResultsLayout->addWidget(new QLabel("Do a search to get started", searchResultsWidget.get()));
    searchResultsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    searchResultsWidget->setLayout(searchResultsLayout.get());
    searchResultsWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    searchResults->setWidgetResizable(true);
    searchResults->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    searchResults->setWidget(searchResultsWidget.get());
    searchResults->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(searchResults.get());
    searchResultsWidget->show();
    setLayout(layout.get());
    connect(searchButton.get(), &QPushButton::clicked, this, &SongSearchWidget::search);
    connect(searchBox.get(), &QLineEdit::returnPressed, this, &SongSearchWidget::search);
    connect(this, &SongSearchWidget::downloadCompleteSignal, this, &SongSearchWidget::downloadComplete);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::thread songSearchThread(&SongSearchWidget::downloadLoop, this);
    songSearchThread.detach();
}

SongSearchWidget::~SongSearchWidget(){
    curl_global_cleanup();
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    std::string data((const char*) ptr, (size_t) size * nmemb);
    *((std::stringstream *) stream) << data;
    return size * nmemb;
}

void SongSearchWidget::downloadLoop(){
    std::string lastSearch;
    while(true){
        std::shared_lock lock(searchTermMutex);
        if(lastSearch == searchTerm) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        lastSearch = searchTerm;
        std::string html;
        auto curl = std::unique_ptr<CURL, std::function<void(CURL*)>>(curl_easy_init(), curl_easy_cleanup);
        if(curl){
            std::replace(searchTerm.begin(), searchTerm.end(), ' ', '+');
            curl_easy_setopt(curl.get(), CURLOPT_URL, (std::string("https://www.youtube.com/results?search_query=") + searchTerm).c_str());
            curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L);
            std::stringstream out;
            curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &out);
            CURLcode res = curl_easy_perform(curl.get());
            if(res != CURLE_OK)
                exit(1);
            html = out.str();
        } else
            return;
        lock.unlock();
        html = (html.substr(html.find("ytInitialData")));
        html = html.substr(html.find('{'), html.find(";</script>") - html.find('{'));
        QJsonObject json = QJsonDocument::fromJson(html.data()).object();
        QJsonArray mainPageContents = json.find("contents").value().toObject()
                .find("twoColumnSearchResultsRenderer").value().toObject()
                .find("primaryContents").value().toObject()
                .find("sectionListRenderer").value().toObject()
                .find("contents").value().toArray().at(0).toObject()
                .find("itemSectionRenderer").value().toObject()
                .find("contents").value().toArray();
        std::vector<std::shared_ptr<Song>> songs;
        for(auto mainPageContent : mainPageContents) {
            QJsonObject videoRenderer;
            if(mainPageContent.toObject().contains("videoRenderer"))
                videoRenderer = mainPageContent.toObject().find("videoRenderer").value().toObject();
            else continue;
            QString title = videoRenderer.find("title").value().toObject().find("runs").value().toArray().first().toObject().find("text").value().toString();
            QString author = videoRenderer.find("ownerText").value().toObject().find("runs").value().toArray().first().toObject().find("text").value().toString();
            QString id = videoRenderer.find("videoId").value().toString();
            songs.emplace_back(std::make_shared<Song>(title, author, QImage(), id));
        }
        downloadCompleteSignal(songs);
    }
}

void SongSearchWidget::search()
{
    std::unique_lock searchLock(searchTermMutex);
    searchTerm = searchBox->text().toStdString();
}

void SongSearchWidget::downloadComplete(const std::vector<std::shared_ptr<Song>> &songs) {
    while(searchResultsLayout->count() > 1){
        auto item = searchResultsLayout->takeAt(0);
        delete item->widget();
        delete item;
    }
    for(const auto& song : songs){
        if(songLoader->isDownloaded(song->getId()))
            searchResultsLayout->insertWidget(searchResultsLayout->count() - 1, new SongSearchItemWidget(songLoader->getById(song->getId()), songLoader, requestedQueue, searchResultsWidget.get()));
        else
            searchResultsLayout->insertWidget(searchResultsLayout->count() - 1, new SongSearchItemWidget(song, songLoader, requestedQueue, searchResultsWidget.get()));
    }
    searchResultsWidget->adjustSize();
}
