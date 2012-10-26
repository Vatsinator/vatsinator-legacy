/*
    worldmap.cpp
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

#include <QtGui>

#include "vatsinatorapplication.h"

#include "glutils/vertexbufferobject.h"
#include "glutils/glextensions.h"

#include "vdebug/glerrors.h"

#include "worldmap.h"
#include "defines.h"

WorldMap::WorldMap() {
  __readDatabase();

  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(glInitialized()),
          this,                                     SLOT(__init()),
          Qt::DirectConnection);
}

WorldMap::~WorldMap() {
#if defined Q_OS_LINUX || defined Q_OS_DARWIN
  delete __worldPolygon.vbo.border;
  delete __worldPolygon.vbo.triangles;
#endif
}

void
WorldMap::draw() const {
#if defined Q_OS_LINUX || defined Q_OS_DARWIN
  __worldPolygon.vbo.border->bind();
  __worldPolygon.vbo.triangles->bind();

  glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
  glDrawElements(GL_TRIANGLES, __worldPolygon.vbo.trianglesSize, GL_UNSIGNED_SHORT, 0); checkGLErrors(HERE);

  __worldPolygon.vbo.triangles->unbind();
  __worldPolygon.vbo.border->unbind();
#else
  glVertexPointer(2, GL_FLOAT, 0, &__worldPolygon.borders[0].x); checkGLErrors(HERE);
  glDrawElements(GL_TRIANGLES, __worldPolygon.triangles.size(), GL_UNSIGNED_SHORT,
                 &__worldPolygon.triangles[0]); checkGLErrors(HERE);
#endif
}

void WorldMap::__readDatabase() {
  QFile db(WORLD_MAP);
  
  if (!db.exists() || !db.open(QIODevice::ReadOnly))
    VatsinatorApplication::alert(
        tr("File") % " " % WORLD_MAP % " " %
        tr("does not exist! Please reinstall the application."),
      true);

  int size;
  db.read(reinterpret_cast< char* >(&size), 4);
  db.seek(4);

  VatsinatorApplication::log("World map polygons: %i.", size);

  QVector< Polygon > polygons;

  polygons.resize(size);
  unsigned allTogether = 0;

  for (int i = 0; i < size; ++i) {
    int counting;
    db.read(reinterpret_cast< char* >(&counting), 4);

    if (counting) {
      polygons[i].borders.resize(counting);
      db.read(reinterpret_cast< char* >(&polygons[i].borders[0].x), sizeof(Point) * counting);
      allTogether += counting;
    }

    db.read(reinterpret_cast< char* >(&counting), 4);

    if (counting) {
      polygons[i].triangles.resize(counting);
      db.read(reinterpret_cast< char* >(&polygons[i].triangles[0]), sizeof(unsigned short) * counting);
    }

  }

  VatsinatorApplication::log("World map coords: %u.", allTogether);

  db.close();

  /* Move all the polygons to one polygon */
  int offset = 0;

  for (Polygon & p: polygons) {
    for (const Point & pt: p.borders)
      __worldPolygon.borders.push_back(pt);

    for (const unsigned short c: p.triangles)
      __worldPolygon.triangles.push_back(c + offset);

    offset += p.borders.size();
  }

}

void
WorldMap::__init() {
#if defined Q_OS_LINUX || defined Q_OS_DARWIN
  VatsinatorApplication::log("Preparing VBOs for WorldMap...");

  __worldPolygon.vbo.border = new VertexBufferObject(GL_ARRAY_BUFFER);
  __worldPolygon.vbo.border->sendData(sizeof(Point) * __worldPolygon.borders.size(),
                                      &__worldPolygon.borders[0].x);

  __worldPolygon.vbo.borderSize = __worldPolygon.borders.size();
  __worldPolygon.borders.clear();

  __worldPolygon.vbo.triangles = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
  __worldPolygon.vbo.triangles->sendData(sizeof(unsigned short) * __worldPolygon.triangles.size(),
                                         &__worldPolygon.triangles[0]);

  __worldPolygon.vbo.trianglesSize = __worldPolygon.triangles.size();
  __worldPolygon.triangles.clear();

  VatsinatorApplication::log("WorldMap's VBOs ready.");
#endif
}


