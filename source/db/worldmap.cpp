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

#include <QtCore>

#include "storage/filemanager.h"
#include "vatsinatorapplication.h"

#include "worldmap.h"

WorldMap::WorldMap(QObject* parent) : QObject(parent) {}

void
WorldMap::initialize() {
  __readDatabase();
}

void WorldMap::__readDatabase() {
  QFile db(FileManager::path("WorldMap.db"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly)) {
    notifyError(tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()));
    return;
  }

  int size;
  db.read(reinterpret_cast<char*>(&size), 4);
  
  if (size) {
    __worldPolygon.borders.resize(size);
    db.read(reinterpret_cast<char*>(__worldPolygon.borders.data()), sizeof(Point) * size);
  }
  
  db.read(reinterpret_cast<char*>(&size), 4);
  if (size) {
    size *= 3;
    __worldPolygon.triangles.resize(size);
    db.read(reinterpret_cast<char*>(__worldPolygon.triangles.data()), sizeof(unsigned int) * size);
  }

  db.close();
}
