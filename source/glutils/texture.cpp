/*
 * texture.cpp
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

#include "glutils/glresourcemanager.h"

#include "texture.h"

Texture::Texture() :
     __id(0) {}

Texture::Texture(const QImage& _img) {
  __id = GlResourceManager::loadImage(_img);
}

Texture::Texture(const QString& _path) {
  __id = GlResourceManager::loadImage(_path);
}

Texture::~Texture() {
  GlResourceManager::deleteImage(__id);
  __id = 0;
}

void
Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, __id);
}

void
Texture::unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}
