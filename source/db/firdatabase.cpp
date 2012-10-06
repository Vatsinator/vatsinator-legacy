/*
    firdatabase.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include <iostream>
#include <fstream>

#include <QtGui>

#include "vatsinatorapplication.h"

#include "firdatabase.h"
#include "defines.h"

FirDatabase::FirDatabase() {
  __readDatabase();

  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(glInitialized()),
          this,                                      SLOT(__init()),
          Qt::DirectConnection);
}


Fir*
FirDatabase::findFirByIcao(const QString& _icao, bool _fss) {
  if (_icao == "ZZZZ")
    return NULL;

  for (Fir & f: __firs)
    if (static_cast< QString >(f.getIcao()) == _icao) {
      if (!f.isOceanic() && _fss)
        continue;

      if (f.isOceanic() && !_fss)
        continue;

      if (f.getTextPosition().x != 0 && f.getTextPosition().y != 0)
        return &f;
      else
        continue;
  }

  return NULL;
}

void
FirDatabase::clearAll() {
  for (Fir & f: __firs)
    f.clear();
}

void
FirDatabase::__readDatabase() {
  __toolTipsPrepared = false;
  __firs.clear();
  
  if (!QFile(FIRS_DB).exists())
    VatsinatorApplication::alert(static_cast< QString >("File ") +
        FIRS_DB + " does not exist! Please reinstsall the application.", true);

  std::fstream db(QString(FIRS_DB).toStdString().c_str(), std::ios::in | std::ios::binary);

  int size;
  db.read(reinterpret_cast< char* >(&size), 4);

  VatsinatorApplication::log("Firs to be read: %i.", size);

  db.seekg(4);
  FirHeader tempHeader;

  __firs.resize(size);

  for (int i = 0; i < size; ++i) {
    db.read(reinterpret_cast< char* >(&tempHeader), sizeof(FirHeader));
    __firs[i].loadHeader(tempHeader);
    int counting;
    db.read(reinterpret_cast< char* >(&counting), sizeof(int));
    __firs[i].getBorders().resize(counting);
    db.read(reinterpret_cast< char* >(&__firs[i].getBorders()[0]), sizeof(Point) * counting);

    db.read(reinterpret_cast< char* >(&counting), sizeof(int));

    if (counting) {
      __firs[i].getTriangles().resize(counting * 3);
      db.read(reinterpret_cast< char* >(&__firs[i].getTriangles()[0]), 2 * counting * 3);
    }
  }

  db.close();

  clearAll();
}

void
FirDatabase::__init() {
  if (__toolTipsPrepared)
    return;

  VatsinatorApplication::log("Preparing VBOs for FIRs...");

  for (Fir & f: __firs)
    f.init();

  __toolTipsPrepared = true;

  VatsinatorApplication::log("FIRs' VBOs prepared.");
}
