/*
 * glresourcemanager.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#ifndef GLRESOURCEMANAGER_H
#define GLRESOURCEMANAGER_H

#include <QtOpenGL>

class GlResourceManager {
  
public:
  /**
   * Converts given image to OpenGL format, loads it and returns
   * its GL-ID.
   * NOTE: You must call GlResourceManager::deleteImage on the image eventually.
   * @param image Already loaded QImage.
   * @return GL's image id in the GPU memory.
   */
  static GLuint loadImage(const QImage&);
  
  /**
   * Loads image from the given path.
   * NOTE: You must call GlResourceManager::deleteImage on the image eventually.
   * @param path Path to the image, can be any Qt-supported type.
   * @return GL's image id in the GPU memory.
   */
  static GLuint loadImage(const QString&);
  
  /**
   * Unloads the image from the GPU memory, frees the pointer.
   * @param img Image handle.
   */
  static void deleteImage(GLuint);
  
#ifndef NO_DEBUG
  static inline unsigned
  textureCount() {
    return __textureCount;
  }
#endif
  
private:
#ifndef NO_DEBUG
  /* For memory tracking */
  static QMap<GLuint, unsigned> __imageMemory;
  static unsigned __textureCount;
#endif
  
};

#endif // GLRESOURCEMANAGER_H
