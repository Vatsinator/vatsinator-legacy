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
#include <QImage>
#include <QString>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
# include "glutils/glresourcemanager.h"
#endif

/**
 * The Texture interface provides a convenient way to manage OpenGL 2D textures.
 * Under Qt5 its implementation uses QOpenGLTexture.
 * When building against Qt4, own Vatsinator's implementation will be used.
 */
class Texture {
  
public:
  
  inline Texture();
  
  /**
   * Destructor frees the memory (destroy() can be called here).
   */
  inline ~Texture();
  
  /**
   * Binds the texture.
   */
  inline void bind();
  
  /**
   * Loads the texture to the memory from the given image data.
   */
  inline void load(const QImage&);
  
  /**
   * Reads the image from the given path and loads it into the memory.
   */
  inline void load(const QString&);
  
  /**
   * Destroys the underlying texture object.
   */
  inline void destroy();
  
  /**
   * Releases the texture.
   */
  inline void release();
  
  /**
   * @return True if the underlying texture object is created and ready to use
   * by OpenGL.
   */
  inline bool isCreated() const;
  
private:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  GLuint __id;
#else
  
#endif

};

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0)) // Qt4 implementation
Texture::Texture() : __id(0) {}

Texture::~Texture() {
  if (__id)
    destroy();
}

void
Texture::bind() {
  Q_ASSERT(isCreated());
  glBindTexture(GL_TEXTURE_2D, __id);
}

void
Texture::load(const QImage& _img) {
  __id = GlResourceManager::loadImage(_img);
}

void
Texture::load(const QString& _path) {
  __id = GlResourceManager::loadImage(_path);
}

void
Texture::destroy() {
  if (__id) {
    GlResourceManager::deleteImage(__id);
    __id = 0;
  }
}

void
Texture::release() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool
Texture::isCreated() const {
  return __id != 0;
}
#else

#endif

#endif // TEXTURE_H
