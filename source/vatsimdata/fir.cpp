/*
    fir.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>

#include "db/firdatabase.h"

#include "glutils/vertexbufferobject.h"
#include "glutils/glextensions.h"
#include "glutils/glresourcemanager.h"

#include "ui/widgets/mapwidget.h"

#include "vatsimdata/models/airporttablemodel.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "debugging/glerrors.h"

#include "fir.h"
#include "defines.h"

Fir::Fir() :
    __icaoTip(0),
    __staff(new ControllerTableModel()),
    __flights(new FlightTableModel()),
    __airports(new AirportTableModel()),
    __uirStaffCount(0) {}

Fir::~Fir() {
  if (__icaoTip)
    GlResourceManager::deleteImage(__icaoTip);

  delete __staff;
  delete __flights;
  delete __airports;

#ifndef CONFIG_NO_VBO

  if (__trianglesVBO)
    delete __trianglesVBO;

  delete __bordersVBO;
#endif
}

void
Fir::addStaff(const Controller* _c) {
  __staff->addStaff(_c);
}

void
Fir::addUirStaff(const Controller* _c) {
  __staff->addStaff(_c);
  __uirStaffCount += 1;
}

void
Fir::addFlight(const Pilot* _p) {
  __flights->addFlight(_p);
}

void
Fir::addAirport(const Airport* _ap) {
  __airports->addAirport(_ap);
}

void
Fir::correctName() {
  if (!__name.contains("Radar") &&
      !__name.contains("Control") &&
      !__name.contains("Radio") &&
      !__name.contains("Oceanic")) {
    if (__oceanic)
      __name += " Oceanic";
    else
      __name += " Center";
  }
}

void
Fir::init() {
  __generateTip();
  __prepareVBO();
}

void
Fir::loadHeader(const FirHeader& _header) {
  __icao = _header.icao;
  __oceanic = static_cast< bool >(_header.oceanic);
  memcpy(__externities, _header.externities, sizeof(Point) * 2);
  __textPosition = _header.textPosition;
}

void
Fir::clear() {
  __staff->clear();
  __flights->clear();
  __airports->clear();
  __uirStaffCount = 0;
}

bool
Fir::isStaffed() const {
  return !__staff->staff().isEmpty() && __uirStaffCount < static_cast< unsigned >(__staff->rowCount());
}

void
Fir::drawBorders() const {
#ifndef CONFIG_NO_VBO
  __bordersVBO->bind();

  glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
  glDrawArrays(GL_LINE_LOOP, 0, __bordersSize); checkGLErrors(HERE);

  __bordersVBO->unbind();
#else
  glVertexPointer(2, GL_FLOAT, 0, &__borders[0].x); checkGLErrors(HERE);
  glDrawArrays(GL_LINE_LOOP, 0, __borders.size()); checkGLErrors(HERE);
#endif
}

void
Fir::drawTriangles() const {
#ifndef CONFIG_NO_VBO

  if (__trianglesSize) {
    __bordersVBO->bind();
    __trianglesVBO->bind();

    glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
    glDrawElements(GL_TRIANGLES, __trianglesSize, GL_UNSIGNED_SHORT, 0); checkGLErrors(HERE);

    __trianglesVBO->unbind();
    __bordersVBO->unbind();
  }

#else

  if (!__triangles.isEmpty()) {
    glVertexPointer(2, GL_FLOAT, 0, &__borders[0].x); checkGLErrors(HERE);
    glDrawElements(GL_TRIANGLES, __triangles.size(), GL_UNSIGNED_SHORT, &__triangles[0]); checkGLErrors(HERE);
  }

#endif
}

GLuint
Fir::__generateTip() const {
  QString icao(__icao);

  if (__oceanic) {
    icao = icao.left(4) + " Oceanic";
  }

  icao = icao.simplified();

  if (__textPosition.x == 0.0 && __textPosition.y == 0.0) {
    __icaoTip = 0;
    return __icaoTip;
  }

  QImage temp(MapWidget::getSingleton().firToolTipBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapWidget::getSingleton().firFont());
  painter.setPen(MapWidget::getSingleton().firPen());
  QRect rectangle(0, 4, 64, 24);
  painter.drawText(rectangle, Qt::AlignCenter | Qt::TextWordWrap, icao);
  __icaoTip = GlResourceManager::loadImage(temp);
  return __icaoTip;
}

void
Fir::__prepareVBO() {
#ifndef CONFIG_NO_VBO
  __bordersVBO = new VertexBufferObject(GL_ARRAY_BUFFER);
  __bordersVBO->sendData(sizeof(Point) * __borders.size(), &__borders[0].x);

  __bordersSize = __borders.size();
  __borders.clear();

  if (!__triangles.isEmpty()) {
    __trianglesVBO = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
    __trianglesVBO->sendData(sizeof(unsigned short) * __triangles.size(), &__triangles[0]);

    __trianglesSize = __triangles.size();
    __triangles.clear();
  } else
    __trianglesVBO = NULL;

#endif
}


