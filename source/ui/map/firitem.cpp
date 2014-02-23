/*
 * firitem.cpp
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

#include "glutils/vertexbufferobject.h"
#include "vatsimdata/fir.h"

#include "firitem.h"
#include "defines.h"

FirItem::FirItem(const Fir* _fir) :
    __fir(_fir),
    __borders(nullptr),
    __triangles(nullptr) {
  __prepareVbo();
}

FirItem::~FirItem() {
  if (__triangles)
    delete __triangles;
  
  delete __borders;
}

void
FirItem::drawBorders() const {
  __borders->bind();
  
  glVertexPointer(2, GL_FLOAT, 0, 0);
  glDrawArrays(GL_LINE_LOOP, 0, __borders->length());
  
  __borders->unbind();
}

void
FirItem::drawBackground() const {
  __borders->bind();
  __triangles->bind();
  
  glVertexPointer(2, GL_FLOAT, 0, 0);
  glDrawElements(GL_TRIANGLES, __triangles->length(), GL_UNSIGNED_SHORT, 0);
  
  __triangles->unbind();
  __borders->unbind();
}

void
FirItem::__prepareVbo() {
  auto& borders = __fir->borders();
  auto& triangles = __fir->triangles();
  
  __borders = new VertexBufferObject(GL_ARRAY_BUFFER);
  __borders->sendData(borders.size() * sizeof(borders[0]), &borders[0]);
  __borders->setLength(borders.size());
  
  if (!triangles.isEmpty()) {
    __triangles = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
    __triangles->sendData(triangles.size() * sizeof(triangles[0]), &triangles[0]);
    __triangles->setLength(triangles.size());
  }
}
