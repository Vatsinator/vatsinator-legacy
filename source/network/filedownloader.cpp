/*
    filedownloader.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore>
#include <QtNetwork>

#include "config.h"
#include "vatsinatorapplication.h"

#include "filedownloader.h"

namespace {
    /**
     * Generates the temporary file name (with the absolute path)
     * from the given url.
     */
    QString fileNameForUrl(const QUrl& url)
    {
        QString baseName = QFileInfo(url.path()).fileName();
    
        Q_ASSERT(!baseName.isEmpty());
        
        if (!QDir(QDir::tempPath()).isReadable()) {
            qWarning("Temporary directory %s is not accessible!",
                    qPrintable(QDir::tempPath()));
            return QString();
        }
        
        auto suffix = [](QString fileName, int n) -> QString {
            if (n <= 1)
                return fileName;
            
            QFileInfo fi(fileName);
            return fi.path() % QDir::separator() % fi.baseName() % "_" % QString::number(n) % "." % fi.completeSuffix();
        };
        
        QString absPath = QDir::tempPath() % "/" % baseName;
        int n = 1;
        while (QFile::exists(suffix(absPath, n))) {
            n += 1;
        }
        
        absPath = suffix(absPath, n);
        qDebug("FileDownloader: file %s will be downloaded to: %s", qPrintable(url.toString()), qPrintable(absPath));
        return absPath;
    }
    
    
    QSharedPointer<QNetworkAccessManager> fileDownloaderNam;
}

FileDownloader::FileDownloader(QObject* parent) :
    QObject(parent),
    __nam(fileDownloaderNam),
    __reply(nullptr)
{
    if (!fileDownloaderNam.data()) {
        fileDownloaderNam.reset(new QNetworkAccessManager);
        __nam = fileDownloaderNam;
    }
    
    connect(__nam.data(), &QNetworkAccessManager::networkAccessibleChanged, [this](QNetworkAccessManager::NetworkAccessibility accessible) {
        if (accessible == QNetworkAccessManager::NotAccessible) {
            qWarning("Network not accessible");
            /* TODO Handle network accessibility */
        }
    });
}

FileDownloader::~FileDownloader()
{

}

void
FileDownloader::fetch(QUrl url)
{
    bool result = QMetaObject::invokeMethod(this, "__fetchImpl", Q_ARG(QUrl, url));
    Q_ASSERT(result);
    Q_UNUSED(result);
}

void
FileDownloader::__startRequest()
{
    Q_ASSERT(!__reply);
    Q_ASSERT(anyTasksLeft());
    
    QUrl url = __urls.front();
    QString fileName = fileNameForUrl(url);
    
    if (fileName.isEmpty()) {
        __startRequest();
        return;
    }
    
    __output.setFileName(fileName);
    
    if (!__output.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit error(tr("Could not open file (%1) for writing!").arg(__output.fileName()), url);
        qCritical("FileDownloader: could not open file (%s) for writing!",
                  qPrintable(__output.fileName()));
        __startRequest();
        return;
    }
    
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Vatsinator/" VATSINATOR_VERSION);
    __reply = __nam->get(request);
    
    connect(__reply, &QNetworkReply::finished, this, &FileDownloader::__finished);
    connect(__reply, &QNetworkReply::readyRead, this, &FileDownloader::__readyRead);
}

void
FileDownloader::__fetchImpl(QUrl url)
{
    __urls.enqueue(url);
    
    if (!__reply)
        __startRequest();
}

void
FileDownloader::__readyRead()
{
    __output.write(__reply->readAll());
}

void
FileDownloader::__finished()
{
    qint64 size = __output.size();
    __output.close();
    
    if (__reply->error()) {
        emit error(tr("Error downloading file: %1").arg(__reply->errorString()), __reply->url());
        qCritical("FileDownloader: error downloading file: %s",
                  qPrintable(__reply->errorString()));
        QFile::remove(__output.fileName());
    } else {
        emit finished(QString(__output.fileName()), __reply->url());
        qDebug("FileDownloader: file %s downloaded, size: %lli",
               qPrintable(__output.fileName()), size);
    }
    
    __urls.dequeue();
    __reply->deleteLater();
    __reply = nullptr;
    
    if (anyTasksLeft())
        __startRequest();
}
