/*
 * airlinedatabase.cpp
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

#include <QtCore>
#include <qjson/parser.h>

#include "db/airline.h"
#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "airlinedatabase.h"
#include "defines.h"

AirlineDatabase::AirlineDatabase(QObject* _parent) :
    QObject(_parent),
    __airlineLogoUrl(),
    __canFetch(false) {}

void
AirlineDatabase::init() {
  QFile db(FileManager::path("data/airlines"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly | QIODevice::Text)) {
    notifyWarning(tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()));
    return;
  }

  QJson::Parser parser;
  bool ok;
  
  QVariant content = parser.parse(&db, &ok);
  QVariantMap rootMap = content.toMap();
  
  if (rootMap.contains("config")) {
    QVariantMap config = rootMap["config"].toMap();
    __airlineLogoUrl = config["airlinelogourl"].toString();
    __canFetch = config["canfetch"].toInt() > 0;
  }
  
  if (rootMap.contains("data")) {
     QVariantList data = rootMap["data"].toList();
     for (const QVariant& a: data) {
       QVariantMap ad = a.toMap();
       Airline* airline = new Airline(
           ad["icao"].toString(),
           ad["name"].toString(),
           ad["country"].toString(),
           ad["website"].toString(),
           ad["logo"].toString(),
           this
         );
       
       __airlines.insert(airline->icao(), airline);
     }
  }
  
  db.close();
}

Airline*
AirlineDatabase::find(const QString& _icao) {
  return __airlines.contains(_icao) ? __airlines[_icao] : nullptr;
}

const Airline*
AirlineDatabase::find(const QString& _icao) const {
  return __airlines.contains(_icao) ? __airlines[_icao] : nullptr;
}
