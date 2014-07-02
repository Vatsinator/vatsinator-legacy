/*
    vertexbufferobject.cpp
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

#include "glutils/glextensions.h"
#include "debugging/glerrors.h"

#include "vertexbufferobject.h"
#include "defines.h"

#ifndef NO_DEBUG
unsigned VertexBufferObject::vboCount = 0;
#endif

VertexBufferObject::VertexBufferObject(GLenum _type):
    __type(_type),
    __length(0) {
  
  static constexpr bool enabled =
#ifndef CONFIG_NO_VBO
    true
#else
    false
#endif
  ;
  
  static_assert(enabled, "VBO disabled in this build config!");
  
  glGenBuffers(1, &__vboID); checkGLErrors(HERE);
  
#ifndef NO_DEBUG
  vboCount += 1;
#endif
}

VertexBufferObject::~VertexBufferObject() {
  glDeleteBuffers(1, &__vboID); checkGLErrors(HERE);
  
#ifndef NO_DEBUG
  unregisterGPUMemoryAllocFunc(__size);
  vboCount -= 1;
#endif
}

void
VertexBufferObject::sendData(unsigned _size, const void* _data) {
  bind();

  glBufferData(__type, _size, NULL, GL_STATIC_DRAW); checkGLErrors(HERE);
  glBufferSubData(__type, 0, _size, _data); checkGLErrors(HERE);

#ifndef NO_DEBUG
  registerGPUMemoryAllocFunc(_size);
  __size = _size;
#endif

  unbind();
  
  __dataSize = _size;
}

void
VertexBufferObject::bind() const {
  glBindBuffer(__type, __vboID); checkGLErrors(HERE);
}

void
VertexBufferObject::unbind() const {
  glBindBuffer(__type, 0); checkGLErrors(HERE);
}

void
VertexBufferObject::setLength(unsigned _length) {
  __length = _length;
}
