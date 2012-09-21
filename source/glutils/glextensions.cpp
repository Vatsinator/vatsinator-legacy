/*
    glextensions.cpp
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

#include <string>
#include <cstddef>
#include <QtOpenGL>

#include <QDebug>
#include <QGLContext>

#include "vdebug/glerrors.h"

#include "config.h"
#if defined Q_OS_LINUX
#include <GL/glx.h>
#elif defined Q_OS_WIN32
#include <windows.h>
#include <wingdi.h>
#elif defined Q_OS_DARWIN
#include <OpenGL/glext.h>
#endif

#ifndef Q_OS_DARWIN
using std::string;

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

void (* glBindBuffer)    (GLenum, GLuint);
void (* glBufferData)    (GLenum, int, const GLvoid*, GLenum);
void (* glBufferSubData) (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
void (* glDeleteBuffers) (GLsizei, const GLuint*);
void (* glGenBuffers)    (GLsizei, GLuint*);

/*
 * Get extension pointer.
 */
template < typename T >
inline T getProcAddress(const string& _procName) {
  T temp = NULL;
#if defined Q_OS_LINUX || defined Q_OS_DARWIN
  temp = reinterpret_cast< T >(glXGetProcAddress((GLubyte*)_procName.c_str()));
#elif defined Q_OS_WIN32
  temp = reinterpret_cast< T >(wglGetProcAddress(_procName.c_str()));
#endif

  Q_ASSERT(temp != (T)NULL);

#ifndef NO_DEBUG
  registerExtensionPointer(_procName.c_str(), reinterpret_cast< long long unsigned >(temp));
#endif

  return temp;
}

void
initGLExtensionsPointers() {
  const QGLContext* context = QGLContext::currentContext();
  Q_ASSERT(context->isValid());

  glBindBuffer = getProcAddress< decltype(glBindBuffer) >("glBindBuffer");
  glBufferData = getProcAddress< decltype(glBufferData) >("glBufferData");
  glBufferSubData = getProcAddress< decltype(glBufferSubData) >("glBufferSubData");
  glDeleteBuffers = getProcAddress< decltype(glDeleteBuffers) >("glDeleteBuffers");
  glGenBuffers = getProcAddress< decltype(glGenBuffers) >("glGenBuffers");
}

#endif // Q_OS_DARWIN

