/*
    vertexbufferobject.h
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


#ifndef VAOOBJECT_H
#define VAOOBJECT_H

#include <QVector>
#include <QtOpenGL>

#include "db/point.h"

class VertexBufferObject {

public:
  VertexBufferObject(GLenum);
  virtual ~VertexBufferObject();

  void sendData(unsigned, const void*);

  void bind() const;
  void unbind() const;
  
  void setLength(unsigned);
  
  inline unsigned
  dataSize() const { return __dataSize; }
  
  inline unsigned
  length() const { return __length; }

#ifndef NO_DEBUG
  static unsigned vboCount;
#endif

private:
  GLuint __vboID;
  GLenum __type;
  unsigned __dataSize;
  unsigned __length;

#ifndef NO_DEBUG
  unsigned __size;
#endif

};

#endif // VAOOBJECT_H
