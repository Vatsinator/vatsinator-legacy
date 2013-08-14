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

#include <QtGui>

#include "vatsimdata/fir.h"

#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "firdatabase.h"
#include "defines.h"

FirDatabase::FirDatabase() {
//   __readDatabase();
  QtConcurrent::run(this, &FirDatabase::__readDatabase);

  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(glInitialized()),
          this,                                     SLOT(__init()),
          Qt::DirectConnection);
}


Fir *
FirDatabase::find(const QString& _icao, bool _fss) {
  if (_icao == "ZZZZ")
    return nullptr;

  for (Fir& f: __firs)
    if (static_cast<QString>(f.icao()) == _icao) {
      if (!f.isOceanic() && _fss)
        continue;

      if (f.isOceanic() && !_fss)
        continue;

      if (f.textPosition().x != 0 && f.textPosition().y != 0)
        return &f;
      else
        continue;
  }

  return nullptr;
}

void
FirDatabase::clearAll() {
  for (Fir& f: __firs)
    f.clear();
}

void
FirDatabase::__readDatabase() {
  __toolTipsPrepared = false;
  __firs.clear();
  
  QFile db(FileManager::path("/WorldFirs.db"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly))
    VatsinatorApplication::alert(
      tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()),
      true);

  int size;
  db.read(reinterpret_cast<char*>(&size), 4);

  VatsinatorApplication::log("Firs to be read: %i.", size);

  db.seek(4);
  FirHeader tempHeader;

  __firs.resize(size);

  for (int i = 0; i < size; ++i) {
    db.read(reinterpret_cast<char*>(&tempHeader), sizeof(FirHeader));
    __firs[i].loadHeader(tempHeader);
    int counting;
    db.read(reinterpret_cast<char*>(&counting), sizeof(int));
    __firs[i].borders().resize(counting);
    db.read(reinterpret_cast<char*>(&__firs[i].borders()[0]), sizeof(Point) * counting);

    db.read(reinterpret_cast<char*>(&counting), sizeof(int));

    if (counting) {
      __firs[i].triangles().resize(counting * 3);
      db.read(reinterpret_cast<char*>(&__firs[i].triangles()[0]), 2 * counting * 3);
    }
  }

  db.close();

  clearAll();
}

void
FirDatabase::__init() {
  Q_ASSERT(!__toolTipsPrepared);

  VatsinatorApplication::log("Preparing VBOs for FIRs...");

  for (Fir& f: __firs)
    f.init();

  __toolTipsPrepared = true;

  VatsinatorApplication::log("FIRs' VBOs prepared.");
}
