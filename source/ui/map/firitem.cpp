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

#include "db/firdatabase.h"
#include "glutils/glresourcemanager.h"
#include "glutils/vertexbufferobject.h"
#include "storage/settingsmanager.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/actions/firdetailsaction.h"
#include "ui/map/mapconfig.h"
#include "ui/userinterface.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"

#include "firitem.h"

FirItem::FirItem(const Fir* _fir, QObject* _parent) :
    QObject(_parent),
    __fir(_fir),
    __position(_fir->data()->header.textPosition.x, _fir->data()->header.textPosition.y),
    __borders(nullptr),
    __triangles(nullptr),
    __label(0) {
  __prepareVbo();
  
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__resetLabel()));
  connect(__fir,                                SIGNAL(updated()),
          this,                                 SLOT(__invalidate()));
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
  
  if (!__label)
    __generateLabel();
  
  glBindTexture(GL_TEXTURE_2D, __label);
  glVertexPointer(2, GL_FLOAT, 0, labelRect);
  glDrawArrays(GL_QUADS, 0, 4);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool
FirItem::needsDrawing() const {
  return !__position.isNull();
}

const LonLat &
FirItem::position() const {
  return __position;
}

QString
FirItem::tooltipText() const {
  QString desc;
  if (!data()->name().isEmpty()) {
    desc.append(data()->name());
    
    if (!data()->country().isEmpty()) {
      desc.append(", ");
      desc.append(data()->country());
    }
  }
  
  QString staff;
  for (const Controller* c: data()->staff()->staff()) {
    staff.append("<br>");
    staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
  }
  
  if (desc.isEmpty() && staff.isEmpty())
    return QString();
  else
    return QString("<p style='white-space:nowrap'><center>" % desc % staff % "</center></p>");
}

QMenu *
FirItem::menu(QWidget* _parent) const {
  QMenu* menu = new QMenu(data()->icao(), _parent);
  
  FirDetailsAction* showFir = new FirDetailsAction(
      data(),
      tr("%1 details").arg(data()->icao()),
      _parent
    );
  connect(showFir,                              SIGNAL(triggered(const Fir*)),
          UserInterface::getSingletonPtr(),     SLOT(showDetailsWindow(const Fir*)));
  menu->addAction(showFir);
  
  for (const Controller* c: data()->staff()->staff()) {
    ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), _parent);
    connect(cda,                                SIGNAL(triggered(const Client*)),
            UserInterface::getSingletonPtr(),   SLOT(showDetailsWindow(const Client*)));
    menu->addAction(cda);
  }
  
  return menu;
}

void
FirItem::showDetailsWindow() const {
  UserInterface::getSingleton().showDetailsWindow(data());
}

void
FirItem::__prepareVbo() {
  auto& borders = __fir->data()->borders;
  auto& triangles = __fir->data()->triangles;
  
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
  
  painter.setFont(SM::get("map.fir_font").value<QFont>());
  
  QColor color = __fir->isStaffed() ? 
    SM::get("map.staffed_fir_borders_color").value<QColor>() :
    SM::get("map.unstaffed_fir_borders_color").value<QColor>();
    
  painter.setPen(color);
  
  painter.drawText(labelRect, Qt::AlignCenter | Qt::TextWordWrap, icao);
  __label = GlResourceManager::loadImage(temp);
}

void
FirItem::__resetLabel() {
  if (__label) {
    GlResourceManager::deleteImage(__label);
    __label = 0;
  }
}

void
FirItem::__invalidate() {
  __resetLabel();
}
