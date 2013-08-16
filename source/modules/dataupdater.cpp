/*
 * dataupdater.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include <QtGui>

#include "network/filedownloader.h"

#include "ui/widgets/dataupdatenotificationwidget.h"

#include "dataupdater.h"
#include "defines.h"

static const int FETCH_DELAY = 10 * 1000;

DataUpdater::DataUpdater(QObject* parent) :
    QObject(parent),
    __notification(nullptr) {
  
  QTimer::singleShot(FETCH_DELAY, this, SLOT(__startDownload()));
}

DataUpdater::~DataUpdater() {
  if (__notification)
    delete __notification;
}

void
DataUpdater::__startDownload() {
  __notification = new DataUpdateNotificationWidget(this);
  FileDownloader* fd = new FileDownloader(__notification->downloadBar());
  connect(fd,   SIGNAL(finished(QString)),
          this, SLOT(__downloadFinished(QString)));
  emit downloading();
  fd->fetch(QUrl("http://vatsinator.eu.org/downloads/latest.zip"));
}

void
DataUpdater::__downloadFinished(QString _fileName) {
  sender()->deleteLater();
  
  this->deleteLater();
}


