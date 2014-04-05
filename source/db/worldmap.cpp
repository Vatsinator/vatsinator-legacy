/*
    worldmap.cpp
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

#include "ui/userinterface.h"
#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "worldmap.h"
#include "defines.h"

WorldMap::WorldMap(QObject* _parent) : QObject(_parent) {
  
  connect(this,                                 SIGNAL(fatal(QString)),
          UserInterface::getSingletonPtr(),     SLOT(fatal(QString)));
  
//   __readDatabase();
  QtConcurrent::run(this, &WorldMap::__readDatabase);
}

void WorldMap::__readDatabase() {
  QFile db(FileManager::path("WorldMap.db"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly))
    emit fatal(tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()));

  int size;
  db.read(reinterpret_cast<char*>(&size), 4);
  db.seek(4);
  
  VatsinatorApplication::log("WorldMap: expected polygons: %i.", size);
  
  QVector<Polygon> polygons;

  polygons.resize(size);
  unsigned allTogether = 0;

  for (int i = 0; i < size; ++i) {
    int counting;
    db.read(reinterpret_cast<char*>(&counting), 4);

    if (counting) {
      polygons[i].borders.resize(counting);
      db.read(reinterpret_cast<char*>(&polygons[i].borders[0].x), sizeof(Point) * counting);
      allTogether += counting;
    }

    db.read(reinterpret_cast<char*>(&counting), 4);

    if (counting) {
      polygons[i].triangles.resize(counting);
      db.read(reinterpret_cast<char*>(&polygons[i].triangles[0]), sizeof(unsigned short) * counting);
    }

  }

  VatsinatorApplication::log("WorldMap: coords count: %u.", allTogether);

  db.close();

  /* Move all the polygons to one polygon */
  int offset = 0;

  for (Polygon& p: polygons) {
    for (const Point& pt: p.borders)
      __worldPolygon.borders.push_back(pt);

    for (const unsigned short c: p.triangles)
      __worldPolygon.triangles.push_back(c + offset);

    offset += p.borders.size();
  }

  VatsinatorApplication::log("WorldMap: done reading database.");
}
