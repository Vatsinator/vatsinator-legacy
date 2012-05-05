/*
    glExtensions.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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
#include <GL/gl.h>

#include <QDebug>

#include "config.h"
#ifdef VATSINATOR_PLATFORM_LINUX
#include <GL/glx.h>
#elif defined VATSINATOR_PLATFORM_WIN32
#include <windows.h>
#include <wingdi.h>
#endif

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
inline T getProcAddress(const char* _procName) {
#ifdef VATSINATOR_PLATFORM_LINUX
	T temp = (T)glXGetProcAddress((GLubyte*)_procName);
#elif defined VATSINATOR_PLATFORM_WIN32
	T temp = (T)wglGetProcAddress(_procName);
#endif
	Q_ASSERT(temp != (T)NULL);
	return temp;
}

void
initGLExtensionsPointers() {
	glBindBuffer = getProcAddress< decltype(glBindBuffer) >("glBindBuffer");
	glBufferData = getProcAddress< decltype(glBufferData) >("glBufferData");
	glBufferSubData = getProcAddress< decltype(glBufferSubData) >("glBufferSubData");
	glDeleteBuffers = getProcAddress< decltype(glDeleteBuffers) >("glDeleteBuffers");
	glGenBuffers = getProcAddress< decltype(glGenBuffers) >("glGenBuffers");
}




