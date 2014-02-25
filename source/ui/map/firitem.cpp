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

#include "glutils/glresourcemanager.h"
#include "glutils/vertexbufferobject.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "firitem.h"
#include "defines.h"

FirItem::FirItem(const Fir* _fir, QObject* _parent) :
    QObject(_parent),
    __fir(_fir),
    __position(_fir->textPosition().x, _fir->textPosition().y),
    __borders(nullptr),
    __triangles(nullptr),
    __label(0) {
  __prepareVbo();
  
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__generateLabel()));
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__generateLabel()));
}

FirItem::~FirItem() {
  if (__label)
    GlResourceManager::deleteImage(__label);
  
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
FirItem::drawLabel() const {
  static const GLfloat labelRect[] = {
    -0.08, -0.05333333,
    -0.08,  0.05333333,
     0.08,  0.05333333,
     0.08, -0.05333333
  };
  
  if (position() == QPointF(0.0, 0.0))
    return;
  
  if (!__label)
    __generateLabel();
  
  glBindTexture(GL_TEXTURE_2D, __label);
  glVertexPointer(2, GL_FLOAT, 0, labelRect);
  glDrawArrays(GL_QUADS, 0, 4);
  glBindTexture(GL_TEXTURE_2D, 0);
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

void
FirItem::__generateLabel() const {
  static QRect labelRect(0, 4, 64, 24);
  
  if (__label)
    GlResourceManager::deleteImage(__label);
  
  QString icao(__fir->icao());
  if (__fir->isOceanic())
    icao = icao.left(4) + " Oceanic";
  
  icao = icao.simplified();
  
  QImage temp(MapConfig::firLabelBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapConfig::firFont());
  
  QColor color;
  if (__fir->isStaffed())
    color = SM::get("colors.staffed_fir_borders").value<QColor>();
  else
    color = SM::get("colors.unstaffed_fir_borders").value<QColor>();
  
  painter.setPen(color);
  
  painter.drawText(labelRect, Qt::AlignCenter | Qt::TextWordWrap, icao);
  __label = GlResourceManager::loadImage(temp);
}
