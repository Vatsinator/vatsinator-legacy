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

#include "db/airline.h"
#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "airlinedatabase.h"

AirlineDatabase::AirlineDatabase(QObject* parent) :
    QObject(parent),
    __airlineLogoUrl(),
    __canFetch(false) {}

void
AirlineDatabase::initialize() {
  QFile db(FileManager::path("data/airlines"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly | QIODevice::Text)) {
    notifyWarning(tr("File %1 could not be opened. Please reinstall the application.").arg(db.fileName()));
    return;
  }

  QJsonParseError error;
  QJsonDocument document = QJsonDocument::fromJson(db.readAll(), &error);
  
  if (error.error != QJsonParseError::NoError) {
    qWarning("AirlineDatabase: the following error occured parsing %s: %s",
             qPrintable(db.fileName()), qPrintable(error.errorString()));
    notifyWarning(tr("File %1 could not be read. Please reinstall the applicaion.").arg(db.fileName()));
    return;
  }
  
  QJsonObject root = document.object();
  if (root.contains("config")) {
    QJsonObject config = root["config"].toObject();
    __airlineLogoUrl = config["airlinelogourl"].toString();
    __canFetch = config["canfetch"].toInt() > 0;
  }
  
  if (root.contains("data")) {
     QJsonArray data = root["data"].toArray();
     for (const QJsonValueRef a: data) {
       Airline* airline = new Airline(a.toObject(), this);
       __airlines.insert(airline->icao(), airline);
     }
  }
  
  db.close();
}

Airline*
AirlineDatabase::find(const QString& icao) {
  return __airlines.contains(icao) ? __airlines[icao] : nullptr;
}

const Airline*
AirlineDatabase::find(const QString& icao) const {
  return __airlines.contains(icao) ? __airlines[icao] : nullptr;
}
