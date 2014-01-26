/*
 * framebufferobject.h
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

#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <QtOpenGL>

class FrameBufferObject {
  
public:
  /**
   * Creates the new FBO with texture and render buffer, from the
   * given dimensions.
   * 
   * @param width Width of the texture.
   * @param height Height of the texture.
   */
  FrameBufferObject(int, int);
  
  /**
   * We can't create FBO of unknown size.
   */
  FrameBufferObject() = delete;
  
  void bind() const;
  void unbind() const;
  
  inline const GLuint texture() const {
    return __textureId;
  }
  
private:
  GLuint __fboId; /* the FBO id */
  GLuint __textureId; /* the texture id */
  GLuint __rboId; /* the render buffer object id */
  
};

#endif // FRAMEBUFFEROBJECT_H
