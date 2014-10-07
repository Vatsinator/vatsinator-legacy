/*
    firdatabase.cpp
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

#include <algorithm>
#include <QtCore>

#include "ui/userinterface.h"
#include "vatsimdata/fir.h"
#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "firdatabase.h"

FirDatabase::FirDatabase(QObject* _parent) : QObject(_parent) {}

void
FirDatabase::initialize() {
  __readDatabase();
}

const FirRecord *
FirDatabase::find(const QString& icao) {
  if (icao == "ZZZZ")
    return nullptr;
  
  auto result = std::lower_bound(__firs.begin(), __firs.end(), icao,
    [](const FirRecord& a, const QString& b) {
      return QString(a.header.icao) < b;
    });
  
  return icao < QString(result->header.icao) ? nullptr : result;
}

void
FirDatabase::__readDatabase() {
  __firs.clear();
  
  QFile db(FileManager::path("WorldFirs.db"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly)) {
    notifyError(tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()));
    return;
  }

  int size;
  db.read(reinterpret_cast<char*>(&size), 4);
  
  qDebug("Firs to be read: %i.", size);
  __firs.resize(size);
  db.seek(4);

  for (int i = 0; i < size; ++i) {
    db.read(reinterpret_cast<char*>(&__firs[i].header), sizeof(FirHeader));
    
    int counting;
    
    db.read(reinterpret_cast<char*>(&counting), sizeof(int));
    __firs[i].borders.resize(counting);
    db.read(reinterpret_cast<char*>(__firs[i].borders.data()), sizeof(Point) * counting);

    db.read(reinterpret_cast<char*>(&counting), sizeof(int));
    if (counting > 0) {
      __firs[i].triangles.resize(counting * 3);
      db.read(reinterpret_cast<char*>(__firs[i].triangles.data()), 2 * counting * 3);
    }
  }
  
  db.close();
  
  std::sort(__firs.begin(), __firs.end(), [](const FirRecord& a, const FirRecord& b) -> bool {
    return QString(a.header.icao) < QString(b.header.icao);
  });
}
