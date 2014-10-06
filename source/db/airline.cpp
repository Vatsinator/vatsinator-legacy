/*
 * airline.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include <QtCore>

#include "db/airlinedatabase.h"
#include "storage/cachefile.h"
#include "storage/filemanager.h"
#include "network/filedownloader.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "airline.h"

static const QString AirlineLogoCacheDir = QStringLiteral("airline-logos");

Airline::Airline(QString icao, QString name, QString country,
                 QString website, QString logo, QObject* parent) :
    QObject(parent),
    __icao(icao),
    __name(name),
    __country(country),
    __website(website),
    __logoUrl(logo) {}

Airline::Airline(const QJsonObject& json, QObject* parent) :
    QObject(parent),
    __icao(json["icao"].toString()),
    __name(json["name"].toString()),
    __country(json["country"].toString()),
    __website(json["website"].toString()),
    __logoUrl(json["logo"].toString()) {}

void
Airline::requestLogo() {
  if (!vApp()->airlineDatabase()->canFetchLogos()) {
    qDebug("Airline: can't fetch logo (%s): disabled", qPrintable(__icao));
    return;
  }
  
  if (__logoUrl.isEmpty())
    return;
  
  QUrl url(vApp()->airlineDatabase()->airlineLogoUrl() % __logoUrl);
  QString fName = QFileInfo(url.path()).fileName();
  fName.prepend(AirlineLogoCacheDir % QDir::separator());
  
  CacheFile cf(fName);
  if (cf.exists()) {
    __logo.load(cf.fileName());
    emit logoFetched();
  } else {
    FileDownloader* fd = new FileDownloader();
    connect(fd,         SIGNAL(finished(QString)),
            this,       SLOT(__logoFetched(QString)));
    fd->fetch(url);
  }
}

void
Airline::__logoFetched(QString fileName) {
  Q_ASSERT(sender());
  
  __logo.load(fileName, "PNG");
  QString newFileName = AirlineLogoCacheDir % QDir::separator() % QFileInfo(fileName).fileName();
  FileManager::moveToCache(fileName, newFileName);
  emit logoFetched();
  
  sender()->deleteLater();
}
