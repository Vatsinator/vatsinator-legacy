/*
    glerrors.cpp
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


#include <QtGui>
#include <GL/gl.h>

QString   glErrors;
long unsigned gpuMemoryUsage = 0;
QMap< QString, long long unsigned > extensions;


QString getErrorString(GLenum _err) {
  switch (_err) {
    case 0x0500:
      return "GL_INVALID_ENUM";
    case 0x0501:
      return "GL_INVALID_VALUE";
    case 0x0502:
      return "GL_INVALID_OPERATION";
    case 0x0503:
      qFatal("Running out of memory!");
      return "GL_OUT_OF_MEMORY";
    case 0x0506:
      return "GL_INVALID_FRAMEBUFFER_OPERATION​";
    default:
      return "unknown error code";
  }
}

void checkGLErrorsFunc(const QString& _at) {
  GLenum err = glGetError(); // fetch errors

  while (err != GL_NO_ERROR) {
    glErrors += _at.section("/", -1, -1) + ": " + getErrorString(err) + "\n";
    err = glGetError();
    qDebug() << "OpenGL error at " << _at << ", code: " << getErrorString(err);
  }
}

void registerGPUMemoryAllocFunc(long unsigned _howMuch) {
  gpuMemoryUsage += _howMuch;
}

void unregisterGPUMemoryAllocFunc(long unsigned _howMuch) {
  gpuMemoryUsage -= _howMuch;
}

void registerExtensionPointer(const QString& _proc, long long unsigned _ptr) {
  extensions[_proc] = _ptr;
}
