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
#include "vatsinatorapplication.h"

#include "approachcircleitem.h"

namespace {
  
  QVector<GLfloat> makeCircle() {
    QVector<GLfloat> circle;
    
    for (qreal angle = 0.0; angle < (2 * M_PI); angle += 0.1) {
      GLfloat x = qCos(angle) * 0.005;
      GLfloat y = qSin(angle) * 0.005;
      circle << x << y;
    }
    
    circle << qCos(0.0) * 0.005 << qSin(0.0) * 0.005;
    
    return qMove(circle);
  }
  
}

ApproachCircleItem::ApproachCircleItem(const Airport* _ap, QObject* _parent) :
    QObject(_parent),
    __position(_ap->data()->longitude, _ap->data()->latitude) {
  
  connect(vApp()->settingsManager(),    SIGNAL(settingsChanged()),
          this,                         SLOT(__reloadSettings()));
}

void
ApproachCircleItem::drawCircle() const {
  /* TODO */
//   if (__circle.isEmpty()) {
//     __circle = makeCircle();
//   }
//   
//   if (!__color.isValid())
//     __getColor();
//   
//   glLineWidth(2.0);
//   glVertexPointer(2, GL_FLOAT, 0, __circle.constData());
//   glColor4f(__color.redF(), __color.greenF(), __color.blueF(), 1.0f);
//   glDrawArrays(GL_LINES, 0, __circle.size() / 2);
//   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//   glLineWidth(1.0);
}

bool
ApproachCircleItem::isVisible() const {
  return false;
}

bool
ApproachCircleItem::isLabelVisible() const {
  return false;
}

const LonLat &
ApproachCircleItem::position() const {
  return __position;
}

void
ApproachCircleItem::drawItem(QOpenGLShaderProgram*) const {

}

void
ApproachCircleItem::drawLabel(QOpenGLShaderProgram*) const {
  
}

void
ApproachCircleItem::drawFocused(QOpenGLShaderProgram*) const {

}

QString
ApproachCircleItem::tooltipText() const {
  return "";
}

void 
ApproachCircleItem::showDetails() const {}

void
ApproachCircleItem::__getColor() const {
  __color = SM::get("map.approach_circle_color").value<QColor>();
}

void ApproachCircleItem::__reloadSettings() {
  __color = QColor();
}

QVector<GLfloat> ApproachCircleItem::__circle;

