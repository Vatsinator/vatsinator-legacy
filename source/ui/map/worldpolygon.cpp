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

#include "glutils/glextensions.h"
#include "ui/map/maprenderer.h"

#include "worldpolygon.h"

WorldPolygon::WorldPolygon() :
    __borders(QOpenGLBuffer::VertexBuffer),
    __triangles(QOpenGLBuffer::IndexBuffer) {
  __initializeBuffers();
}

WorldPolygon::~WorldPolygon() {
  __triangles.destroy();
  __borders.destroy();
  __vao.destroy();
}

void
WorldPolygon::paint() {
  __vao.bind();
  glDrawElements(GL_TRIANGLES, __vertices, GL_UNSIGNED_INT, 0);
  __vao.release();
}

void
WorldPolygon::__initializeBuffers() {
  const QVector<Point>& bordersData = WorldMap::getSingleton().borders();
  Q_ASSERT(!bordersData.isEmpty());
  
  const QVector<unsigned int>& trianglesData = WorldMap::getSingleton().triangles();
  Q_ASSERT(!trianglesData.isEmpty());
  
  __vao.create();
  Q_ASSERT(__vao.isCreated());
  __vao.bind();
  
  __borders.create();
  Q_ASSERT(__borders.isCreated());
  __borders.setUsagePattern(QOpenGLBuffer::StaticDraw);
  __borders.bind();
  __borders.allocate(bordersData.constData(), sizeof(Point) * bordersData.size());
  
  __triangles.create();
  Q_ASSERT(__triangles.isCreated());
  __triangles.setUsagePattern(QOpenGLBuffer::StaticDraw);
  __triangles.bind();
  __triangles.allocate(trianglesData.constData(), sizeof(unsigned int) * trianglesData.size());
  
  __borders.bind();
  __triangles.bind();
  glVertexAttribPointer(MapRenderer::vertexLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(MapRenderer::vertexLocation());
  __vao.release();
  __borders.release();
  __triangles.release();
  
  __vertices = WorldMap::getSingleton().triangles().size();
}
