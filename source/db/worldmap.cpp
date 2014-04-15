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

#include "glutils/vertexbufferobject.h"
#include "glutils/glextensions.h"
#include "debugging/glerrors.h"
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
  
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(glInitialized()),
          this,                                     SLOT(__init()),
          Qt::DirectConnection);
}

WorldMap::~WorldMap() {
#ifndef CONFIG_NO_VBO
  delete __worldPolygon.vbo.border;
  delete __worldPolygon.vbo.triangles;
#endif
}

void
WorldMap::draw() const {
#ifndef CONFIG_NO_VBO
  __worldPolygon.vbo.border->bind();
  __worldPolygon.vbo.triangles->bind();

  glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
  glDrawElements(GL_TRIANGLES, __worldPolygon.vbo.trianglesSize, GL_UNSIGNED_INT, 0); checkGLErrors(HERE);

  __worldPolygon.vbo.triangles->unbind();
  __worldPolygon.vbo.border->unbind();
#else
  glVertexPointer(2, GL_FLOAT, 0, __worldPolygon.borders.constData()); checkGLErrors(HERE);
  glDrawElements(GL_TRIANGLES, __worldPolygon.triangles.size(), GL_UNSIGNED_INT,
                 __worldPolygon.triangles.constData()); checkGLErrors(HERE);
#endif
}

void WorldMap::__readDatabase() {
  QFile db(FileManager::path("WorldMap.db"));
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly))
    emit fatal(tr("File %1 could not be opened! Please reinstall the application.").arg(db.fileName()));

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

void
WorldMap::__init() {
#ifndef CONFIG_NO_VBO
  VatsinatorApplication::log("Preparing VBOs for WorldMap...");

  __worldPolygon.vbo.border = new VertexBufferObject(GL_ARRAY_BUFFER);
  __worldPolygon.vbo.border->sendData(sizeof(Point) * __worldPolygon.borders.size(),
                                      __worldPolygon.borders.constData());

  __worldPolygon.vbo.borderSize = __worldPolygon.borders.size();
  __worldPolygon.borders.clear();

  __worldPolygon.vbo.triangles = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
  __worldPolygon.vbo.triangles->sendData(sizeof(unsigned int) * __worldPolygon.triangles.size(),
                                         __worldPolygon.triangles.constData());

  __worldPolygon.vbo.trianglesSize = __worldPolygon.triangles.size();
  __worldPolygon.triangles.clear();

  VatsinatorApplication::log("WorldMap's VBOs ready.");
#endif
}


