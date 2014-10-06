/*
 * worldpolygon.h
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

#ifndef WORLDPOLYGON_H
#define WORLDPOLYGON_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class MapRenderer;

/**
 * The WorldPolygon class stores the world map in OpenGL buffers and
 * renders it when needed.
 */
class WorldPolygon {
  
public:
  /**
   * Fills the buffers with data. _renderer_ provides OpenGL rendering
   * context.
   */
  WorldPolygon(MapRenderer* renderer);
  
  ~WorldPolygon();
  
  /**
   * Renders the map.
   * Only lands are being rendered; their color must be specified beforehand.
   */
  void paint();
  
private:
  void __initializeBuffers();
  
  MapRenderer* __renderer;
  
  QOpenGLBuffer __borders;
  QOpenGLBuffer __triangles;
  QOpenGLVertexArrayObject __vao;
  int __vertices;

};

#endif // WORLDPOLYGON_H
