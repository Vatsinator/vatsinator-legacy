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

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

#include <QtNetwork>

#include "config.h"
#include "vatsinatorapplication.h"

#include "filedownloader.h"

FileDownloader::FileDownloader(QProgressBar* _pb, QObject* _parent) :
    QObject(_parent),
    __pb(_pb), 
    __reply(nullptr) {}

void
FileDownloader::fetch(const QUrl& _url) {
  __urls.enqueue(_url);
  
  if (!__reply)
    __startRequest();
}

QString
FileDownloader::fileNameForUrl(const QUrl& _url) {
  QString baseName = QFileInfo(_url.path()).fileName();
  
  Q_ASSERT(!baseName.isEmpty());
  
  if (!QDir(QDir::tempPath()).isReadable()) {
    emit error(tr("Temporary directory (%1) is not readable!").arg(QDir::tempPath()));
    qWarning("Temporary directory %s is not accessible!",
                               qPrintable(QDir::tempPath()));
    return QString();
  }
  
  QString absPath = QDir::tempPath() % "/" % baseName;
  
  qDebug("FileDownloader: file %s will be downloaded to: %s", qPrintable(_url.toString()), qPrintable(absPath));
  
  if (QFile::exists(absPath))
    QFile(absPath).remove();
  
  return absPath;
}

void
FileDownloader::__startRequest() {
  if (__reply || !anyTasksLeft())
    return;
  
  QUrl url = __urls.dequeue();
  QString fileName = fileNameForUrl(url);
  
  if (fileName.isEmpty()) {
    __startRequest();
    return;
  }
  
  __output.setFileName(fileName);
  if (!__output.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    emit error(tr("Could not open file (%1) for writing!").arg(__output.fileName()));
    qCritical("FileDownloader: could not open file (%s) for writing!",
              qPrintable(__output.fileName()));
    __startRequest();
    return;
  }
  
  QNetworkRequest request(url);
  request.setRawHeader("User-Agent", "Vatsinator/" VATSINATOR_VERSION);
  __reply = __nam.get(request);
  
  connect(__reply,      SIGNAL(finished()),
          this,         SLOT(__finished()));
  connect(__reply,      SIGNAL(readyRead()),
          this,         SLOT(__readyRead()));
  
  if (__pb) {
    connect(__reply,    SIGNAL(downloadProgress(qint64,qint64)),
            this,       SLOT(__updateProgress(qint64, qint64)));
    __pb->setEnabled(true);
  }
}

void
FileDownloader::__readyRead() {
  __output.write(__reply->readAll());
}

void
FileDownloader::__finished() {
  qint64 size = __output.size();
  __output.close();
  
  if (__pb) {
    __pb->reset();
    __pb->setEnabled(false);
  }
  
  if (__reply->error()) {
    emit error(tr("Error downloading file: %1").arg(__reply->errorString()));
    qCritical("FileDownloader: error downloading file: %s",
              qPrintable(__reply->errorString()));
    QFile::remove(__output.fileName());
  } else {
    emit finished(QString(__output.fileName()));
    qCritical("FileDownloader: file %s downloaded, size: %lli",
              qPrintable(__output.fileName()), size);
  }
  
  __reply->deleteLater();
  __reply = nullptr;
  
  __startRequest();
}

void
FileDownloader::__updateProgress(qint64 _read, qint64 _total) {
  __pb->setMaximum(_total);
  __pb->setValue(_read);
}