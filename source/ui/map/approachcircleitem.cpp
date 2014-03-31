/*
 * approachcircleitem.cpp
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

#include <qmath.h>

#include "db/airportdatabase.h"
#include "storage/settingsmanager.h"
#include "vatsimdata/airport.h"

#include "approachcircleitem.h"
#include "defines.h"

namespace {
  
  QVector<GLfloat> makeCircle() {
    QVector<GLfloat> circle;
    
    for (qreal angle = 0.0; angle <= (2 * M_PI); angle += 0.1) {
      GLfloat x = qCos(angle) * 0.005;
      GLfloat y = qSin(angle) * 0.005;
      circle << x << y;
    }
    
    circle << qCos(0.0) * 0.005 << qSin(0.0) * 0.005;
    
    return std::move(circle);
  }
  
}

ApproachCircleItem::ApproachCircleItem(const Airport* _ap, QObject* _parent) :
    QObject(_parent),
    __position(_ap->data()->longitude, _ap->data()->latitude) {
  
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadSettings()));
}

void
ApproachCircleItem::drawCircle() const {
  if (__circle.isEmpty()) {
    __circle = makeCircle();
    __circle.prepend(0.0f);
    __circle.prepend(0.0f);
  }
  
  if (__colors.isEmpty())
    __fillColors();
  
  glEnableClientState(GL_COLOR_ARRAY);
  
  glVertexPointer(2, GL_FLOAT, 0, __circle.constData());
  glColorPointer(4, GL_FLOAT, 0, __colors.constData());
  
  glDrawArrays(GL_TRIANGLE_FAN, 0, __circle.size() / 2);
  
  glDisableClientState(GL_COLOR_ARRAY);
}

bool
ApproachCircleItem::needsDrawing() const {
  return false;
}

const LonLat &
ApproachCircleItem::position() const {
  return __position;
}

QString
ApproachCircleItem::tooltipText() const {
  return "";
}

QMenu *
ApproachCircleItem::menu(QWidget*) const {
  return nullptr;
}

void 
ApproachCircleItem::showDetailsWindow() const {}

void
ApproachCircleItem::__fillColors() const {
  QColor outer = SM::get("map.approach_circle_color").value<QColor>();
  QColor inner = outer.lighter();
  
  __colors << inner.redF() << inner.greenF() << inner.blueF() << 0.1f;
  
  for (int i = 2; i < __circle.size(); i += 2) {
    __colors << outer.redF() << outer.greenF() << outer.blueF() << 0.5f;
  }
}

void ApproachCircleItem::__reloadSettings() {
  __colors.clear();
}

QVector<GLfloat> ApproachCircleItem::__circle;

