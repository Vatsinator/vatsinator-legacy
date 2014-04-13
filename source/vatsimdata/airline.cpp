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
#include <qjson/parser.h>

#include "db/airlinedatabase.h"
#include "storage/cachefile.h"
#include "storage/filemanager.h"
#include "network/filedownloader.h"
#include "network/plaintextdownloader.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "airline.h"
#include "defines.h"

static const QString AirlineDataUrl =
  QString(NetConfig::Vatsinator::apiUrl()) %
  QString("airlinedata.php?icao=%1");
static const QString AirlineLogoCacheDir = 
  "airline-logos";

Airline::Airline(QString _icao, QObject* _parent): QObject(_parent), __icao(_icao) {
  __name = AirlineDatabase::getSingleton().find(_icao);
  __isValid = !__name.isEmpty();
}

void
Airline::fetchData() {
  PlainTextDownloader* d = new PlainTextDownloader();
  connect(d,    SIGNAL(finished(QString)),
          this, SLOT(__dataFetched(QString)));
  d->fetchData(AirlineDataUrl.arg(__icao));
}

void
Airline::fetchLogo(QString _url) {
  QUrl url(_url);
  QString fName = QFileInfo(url.path()).fileName();
  fName.prepend(AirlineLogoCacheDir % QDir::separator());
  
  CacheFile cf(fName);
  if (cf.exists()) {
    __logo.load(cf.fileName());
    emit dataUpdated();
  } else {
    FileDownloader* fd = new FileDownloader();
    connect(fd,         SIGNAL(finished(QString)),
            this,       SLOT(__logoFetched(QString)));
    fd->fetch(_url);
  }
}

void
Airline::__dataFetched(QString _data) {
  Q_ASSERT(sender());
  
  QJson::Parser parser;
  bool ok;
  
  QVariant content = parser.parse(_data.toUtf8(), &ok);
  if (!ok) {
    VatsinatorApplication::log("Airline (%s): error parsing response", qPrintable(__icao));
    return;
  }
  
  QVariantMap map = content.toMap();
  if (!map["result"].canConvert(QVariant::Int) || map["result"].toInt() != 1) {
    VatsinatorApplication::log("Airline (%s): error: %s", qPrintable(__icao), qPrintable(map["reason"].toString()));
    return;
  }
  
  if (map.contains("data")) {
    QVariantMap data = map["data"].toMap();
    __country = data["country"].toString();
    __website = data["website"].toString();
    
    QString logoUrl = data["logo"].toString();
    if (!logoUrl.isEmpty())
      fetchLogo(logoUrl);
    
    emit dataUpdated();
  }
  
  sender()->deleteLater();
}

void
Airline::__logoFetched(QString _fileName) {
  Q_ASSERT(sender());
  
  __logo.load(_fileName, "PNG");
  QString newFileName = AirlineLogoCacheDir % QDir::separator() % QFileInfo(_fileName).fileName();
  FileManager::moveToCache(_fileName, newFileName);
  emit dataUpdated();
  
  sender()->deleteLater();
}
