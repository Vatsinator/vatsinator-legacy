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
 * texture in a most efficient way.
 */
class Texture {
  
public:
  
  /**
   * Create empty texture, with id = 0.
   */
  Texture();
  
  /**
   * Create a new texture from the given image.
   */
  Texture(const QImage&);
  
  /**
   * Create a new texture from the given image path.
   */
  Texture(const QString&);
  
  /**
   * Destructor removes the texture from the memory.
   */
  virtual ~Texture();
  
  /**
   * Binds the texture.
   */
  void bind() const;
  
  /**
   * Unbinds any texture.
   */
  static void unbind();
  
private:
  GLuint __id;

};

#endif // TEXTURE_H
