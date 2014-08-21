/*
 * texture.h
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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>
#include <QString>
#include <QImage>

/**
 * The Texture class is responsible for handling a single OpenGL
 * texture.
 * It is used as a substitute of QOpenGLTexture in Qt4 builds.
 */
class Texture {
  
public:
  
  enum Target {
    Target2D
  };
  
  /**
   * Create empty texture, with id = 0.
   */
  Texture(Target);
  
  /**
   * Destructor removes the texture from the memory.
   */
  virtual ~Texture();
  
  /**
   * Binds the texture.
   */
  void bind();
  
  /**
   * Destroys the underlying texture object.
   */
  void destroy();
  
  /**
   * Unbinds any texture.
   */
  static void release();
  
  inline bool isCreated() const { return __id; }
  
private:
  GLuint __id;

};

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
typedef Texture QOpenGLTexture
#endif

#endif // TEXTURE_H
