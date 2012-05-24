/*
    glextensions.h
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


#ifndef GLEXTENSIONS_H
#define GLEXTENSIONS_H

#include <cstddef>
#include <GL/gl.h>

#define GL_STATIC_DRAW 0x88E4
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

extern void (* glBindBuffer)    (GLenum, GLuint);
extern void (* glBufferData)    (GLenum, int, const GLvoid*, GLenum);
extern void (* glBufferSubData) (GLenum, GLintptr, GLsizeiptr, const GLvoid*);
extern void (* glDeleteBuffers) (GLsizei, const GLuint*);
extern void (* glGenBuffers)    (GLsizei, GLuint*);

void initGLExtensionsPointers();



#endif // GLEXTENSIONS_H
