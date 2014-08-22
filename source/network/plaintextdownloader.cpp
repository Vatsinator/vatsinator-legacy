/*
    plaintextdownloader.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include <QtNetwork>

#include "config.h"
#include "vatsinatorapplication.h"

#include "plaintextdownloader.h"

PlainTextDownloader::PlainTextDownloader(QProgressBar* _pb, QObject* _parent) :
    QObject(_parent),
    __progressBar(_pb),
    __reply(nullptr) {}

void
PlainTextDownloader::fetchData(const QString& _url) {
  __urls.enqueue(_url);

  if (!__reply)
    __startRequest();
}

void
PlainTextDownloader::abort() {
  if (__reply) {
    __reply->abort();
    __reply->deleteLater();
    __reply = nullptr;
  
    if (anyTasksLeft())
      __startRequest();
  }
}

void
PlainTextDownloader::__startRequest() {
  if (!__urls.isEmpty()) {
    QNetworkRequest request(__urls.dequeue());
    request.setRawHeader("User-Agent", "Vatsinator/" VATSINATOR_VERSION);
    __reply = __nam.get(request);
  } else {
    return;
  }
  
  __temp.clear(); 

  connect(__reply, SIGNAL(finished()), this, SLOT(__finished()));
  connect(__reply, SIGNAL(readyRead()), this, SLOT(__readyRead()));
  
  if (__progressBar) {
    connect(__reply, SIGNAL(downloadProgress(qint64, qint64)),
            this,    SLOT(__updateProgress(qint64, qint64)));
    __progressBar->setEnabled(true);
  }
}

void
PlainTextDownloader::__readyRead() {
  __temp.append(__reply->readAll());
}

void
PlainTextDownloader::__finished() {
  
  if (__progressBar) {
    __progressBar->reset();
    __progressBar->setEnabled(false);
  }
  
  if (__reply->error() == QNetworkReply::NoError) {
    qDebug("PlainTextDownloader: %s: finished",
           qPrintable(__reply->url().toString()));
    __data = __temp;
    emit finished(__data);
  } else {
    qWarning("PlainTextDownloader: %s: error (%s)",
             qPrintable(__reply->url().toString()),
             qPrintable(__reply->errorString()));
    
    emit error();
  }
  
  __reply->deleteLater();
  __reply = nullptr;
  
  if (anyTasksLeft())
    __startRequest();
}

void
PlainTextDownloader::__updateProgress(qint64 _bRead, qint64 _bTotal) {
  __progressBar->setMaximum(_bTotal);
  __progressBar->setValue(_bRead);
}
