/*
 * vertexarrayobject.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef VERTEXARRAYOBJECT_H
#define VERTEXARRAYOBJECT_H

class VertexArrayObject {

public:
  inline VertexArrayObject();
  
  inline ~VertexArrayObject();
  
  inline void create();
  
  inline void bind();
  
  inline void release();
  
  inline void destroy();
  
private:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  GLuint __id;
#else
  
#endif
};

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0)) // Qt4 implementation
VertexArrayObject::VertexArrayObject() : __id(0) {}

VertexArrayObject::~VertexArrayObject() {
  if (__id)
    destroy();
}

void
VertexArrayObject::create() {
  
}

#else


#endif

#endif // VERTEXARRAYOBJECT_H
