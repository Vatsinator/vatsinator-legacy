/*
    glextensions.cpp
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

#include <cstddef>

#include <QtOpenGL>
#include <QDebug>
#include <QGLContext>

#include "debugging/glerrors.h"

#include "config.h"
#if defined Q_OS_LINUX
# include <GL/glx.h>
#elif defined Q_OS_WIN32
# include <windows.h>
# include <wingdi.h>
#elif defined Q_OS_DARWIN
# include <OpenGL/glext.h>
#endif

#ifndef Q_WS_MAC // apple's impelementation already knows these extensions

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

void (* glBindBuffer)    (GLenum, GLuint);
void (* glBufferData)    (GLenum, int, const GLvoid*, GLenum);
void (* glBufferSubData) (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
void (* glDeleteBuffers) (GLsizei, const GLuint*);
void (* glGenBuffers)    (GLsizei, GLuint*);
void (* glGenFramebuffers)                       (GLsizei, GLuint*);
void (* glDeleteFramebuffers)                    (GLsizei, GLuint*);
void (* glBindFramebuffer)                       (GLenum, GLuint);
GLenum (* glCheckFramebufferStatus)              (GLenum);
void (* glGetFramebufferAttachmentParameteriv)   (GLenum, GLenum, GLenum, GLint*);
void (* glGenerateMipmap)                        (GLenum);
void (* glFramebufferTexture2D)                  (GLenum, GLenum, GLenum, GLuint, GLint);
void (* glFramebufferRenderbuffer)               (GLenum, GLenum, GLenum, GLuint);
void (* glGenRenderbuffers)                      (GLsizei, GLuint*);
void (* glDeleteRenderbuffers)                   (GLsizei, const GLuint*);
void (* glBindRenderbuffer)                      (GLenum, GLuint);
void (* glRenderbufferStorage)                   (GLenum, GLenum, GLsizei, GLsizei);
void (* glGetRenderbufferParameteriv)            (GLenum, GLenum, GLint*);
GLboolean (* glIsRenderbuffer)                   (GLuint);

/*
 * Get extension pointer.
 */
template <typename T>
 static inline T
 getProcAddress(const char* _procName) {
   T temp = NULL;
#if defined Q_WS_X11
    temp = reinterpret_cast<T>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(_procName)));
#elif defined Q_WS_WIN
    temp = reinterpret_cast<T>(wglGetProcAddress(_procName));
#endif
    
    Q_ASSERT(temp != nullptr);

#ifndef NO_DEBUG
    registerExtensionPointer(_procName, reinterpret_cast<long long unsigned>(temp));
#endif
  
    return temp;
  }

void
initGLExtensionsPointers() {
  Q_ASSERT(QGLContext::currentContext()->isValid());
  
#define init_ext(val) val = getProcAddress<decltype(val)>(#val)

  init_ext(glBindBuffer);
  init_ext(glBufferData);
  init_ext(glBufferSubData);
  init_ext(glDeleteBuffers);
  init_ext(glGenBuffers);
  
  init_ext(glGenFramebuffers);
  init_ext(glDeleteFramebuffers);
  init_ext(glBindFramebuffer);
  init_ext(glCheckFramebufferStatus);
  init_ext(glGetFramebufferAttachmentParameteriv);
  init_ext(glGenerateMipmap);
  init_ext(glFramebufferTexture2D);
  init_ext(glFramebufferRenderbuffer);
  init_ext(glGenRenderbuffers);
  init_ext(glDeleteRenderbuffers);
  init_ext(glBindRenderbuffer);
  init_ext(glRenderbufferStorage);
  init_ext(glGetRenderbufferParameteriv);
  init_ext(glIsRenderbuffer);
  
#undef init_ext
}

#endif // Q_WS_MAC

