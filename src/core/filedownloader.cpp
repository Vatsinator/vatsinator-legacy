/*
 * filedownloader.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "filedownloader.h"
#include <QtCore>
#include <QtNetwork>

static QThreadStorage<QNetworkAccessManager*> networkAccessManagers;

namespace Vatsinator { namespace Core {

FileDownloader::FileDownloader(QObject* parent) :
    QObject(parent) {}

FileDownloader::~FileDownloader() {}

void FileDownloader::fetch(const QUrl& url)
{
    if (!networkAccessManagers.hasLocalData())
        networkAccessManagers.setLocalData(new QNetworkAccessManager);
    
    if (m_isDownloading)
        qWarning("ATTENTION: this FileDownloader instance is already downloading something");
    
    m_isDownloading = true;
    
    QNetworkRequest request(url);
    QString version = QCoreApplication::applicationVersion();
    if (version.isEmpty())
        version = QStringLiteral("1.0");
    
    QString userAgent = QStringLiteral("Vatsinator/%1").arg(version);
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    m_retries = 0;
    start(request);
}

void FileDownloader::setMaxRetries(int maxTrials)
{
    m_maxRetries = maxTrials;
}

void FileDownloader::start(const QNetworkRequest& request)
{
    m_retries += 1;

    QNetworkAccessManager* nam = networkAccessManagers.localData();
    QNetworkReply* reply = nam->get(request);
    connect(reply, &QNetworkReply::finished, this, &FileDownloader::finish);
    connect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::progress);
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
        [](QNetworkReply::NetworkError code){
//            qFatal("FileDownloader: error %d", code);
        }
    );
}

void FileDownloader::finish()
{
    QSharedPointer<QNetworkReply> reply(qobject_cast<QNetworkReply*>(sender()), &QObject::deleteLater);
    Q_ASSERT(reply);
    
    m_isDownloading = false;
    
    if (reply->error()) {
        if (m_retries >= m_maxRetries) {
            qCritical("FileDownloader: error downloading file: %s",
                     qPrintable(reply->errorString()));
            emit error(tr("Error downloading file: %1").arg(reply->errorString()), reply->url());
        } else {
            start(reply->request());
        }
    } else {
        m_data = reply->readAll();
        qDebug("FileDownloader: %s downloaded", qPrintable(reply->url().toString()));
        emit finished(reply->url());
    }
}

}} /* namespace Vatsinator::Core */
