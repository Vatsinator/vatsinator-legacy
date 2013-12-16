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
#include <QRegExp>

#include "storage/filemanager.h"

#include "vatsinatorapplication.h"

#include "airlinedatabase.h"
#include "defines.h"

AirlineDatabase::AirlineDatabase() {
  QtConcurrent::run(this, &AirlineDatabase::__init);
}

const QString
AirlineDatabase::find(const QString& _key) {
  return __airlines.value(_key, __nope);
}

void
AirlineDatabase::__init() {
  QFile db(FileManager::path("data/airlines"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly | QIODevice::Text)) {
    VatsinatorApplication::alert(
      tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()));
    return;
  }
  
  static QRegExp rx("^(.{3})\\s(.+)$");
  
  while (!db.atEnd()) {
    QString line = db.readLine();
    line = line.simplified();
    
    if (line.startsWith('#') || line.isEmpty())
      continue;
    
    if (rx.indexIn(line) >= 0)
      __airlines.insert(rx.cap(1), rx.cap(2));
    else
      VatsinatorApplication::log("AirlineDatabase: invalid entry: %s", qPrintable(line));
  }
  
  db.close();
}
