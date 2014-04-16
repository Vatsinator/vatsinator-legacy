/*
 * worldpolygon.cpp
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

#include <QtOpenGL>

#include "db/worldmap.h"

#include "debugging/glerrors.h"

#include "glutils/glextensions.h"
#include "glutils/vertexbufferobject.h"

#include "worldpolygon.h"
#include "defines.h"

WorldPolygon::WorldPolygon() {
  __createVbos();
}

WorldPolygon::~WorldPolygon() {
#ifndef CONFIG_NO_VBO
  delete __borders;
  delete __triangles;
#endif
}

void
WorldPolygon::paint() {
#ifndef CONFIG_NO_VBO
  __borders->bind();
  __triangles->bind();
  
  glVertexPointer(2, GL_FLOAT, 0, 0);
  glDrawElements(GL_TRIANGLES, WorldMap::getSingleton().triangles().size(), GL_UNSIGNED_INT, 0);
  checkGLErrors(HERE);
  
  __borders->unbind();
  __triangles->unbind();
#else
# error "Not implemented"
#endif
}

void
WorldPolygon::__createVbos() {
#ifndef CONFIG_NO_VBO
  const QVector<Point>& bordersData = WorldMap::getSingleton().borders();
  Q_ASSERT(!bordersData.isEmpty());
  
  const QVector<unsigned int>& trianglesData = WorldMap::getSingleton().triangles();
  Q_ASSERT(!trianglesData.isEmpty());
  
  __borders = new VertexBufferObject(GL_ARRAY_BUFFER);
  __borders->sendData(sizeof(Point) * bordersData.size(), bordersData.constData());
  
  __triangles = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
  __triangles->sendData(sizeof(unsigned int) * trianglesData.size(), trianglesData.constData());
#endif
}
