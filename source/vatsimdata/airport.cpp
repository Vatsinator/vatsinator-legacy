/*
    airport.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "ui/mapwidget/mapwidget.h"

#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "vatsinatorapplication.h"

#include "airport.h"
#include "defines.h"

Airport::Airport(const QString& _icao) :
    __data(AirportDatabase::GetSingleton().find(_icao)),
    __staff(new ControllerTableModel()),
    __inbounds(new FlightTableModel()),
    __outbounds(new FlightTableModel()),
    __labelTip(0),
    __firs( {NULL, NULL}) {
  if (!__data)
    VatsinatorApplication::log("Airport %s not found!", _icao.toStdString().c_str());

  if (__data) {
    Fir* f = FirDatabase::GetSingleton().findFirByIcao(QString(__data->fir_a), __data->is_fir_a_oceanic);

    if (f) {
      f->addAirport(this);
    }

    __firs[0] = f;

    f = FirDatabase::GetSingleton().findFirByIcao(QString(__data->fir_b), __data->is_fir_b_oceanic);

    if (f)
      f->addAirport(this);

    __firs[1] = f;

  }

  //__generateTip();
}

Airport::~Airport() {
  if (__labelTip)
    MapWidget::deleteImage(__labelTip);

  delete __staff;
  delete __inbounds;
  delete __outbounds;
}

void
Airport::addStaff(const Controller* _c) {
  __staff->addStaff(_c);
}

void
Airport::addInbound(const Pilot* _p) {
  __inbounds->addFlight(_p);
}

void
Airport::addOutbound(const Pilot* _p) {
  __outbounds->addFlight(_p);
}

unsigned
Airport::countDepartures() const {
  unsigned i = 0;

  for (const Pilot * p: __outbounds->getFlights())
    if (p->flightStatus == DEPARTING)
      ++i;

  return i;
}

unsigned
Airport::countOutbounds() const {
  return __outbounds->rowCount();
}

unsigned
Airport::countArrivals() const {
  unsigned i = 0;

  for (const Pilot * p: __inbounds->getFlights())
    if (p->flightStatus == ARRIVED)
      ++i;

  return i;
}

unsigned
Airport::countInbounds() const {
  return __inbounds->rowCount();
}

bool
Airport::hasApproach() const {
  for (const Controller * c: __staff->getStaff())
    if (c->facility == APP)
      return true;

  return false;
}

unsigned
Airport::getFacilities() const {
  unsigned facilities = 0;

  for (const Controller * c: __staff->getStaff()) {
    facilities |= c->facility;
  }

  return facilities;
}

void
Airport::drawLines() const {
  for (const Pilot * p: __inbounds->getFlights())
    if (p->flightStatus != ARRIVED)
      p->drawLineTo();

  for (const Pilot * p: __outbounds->getFlights())
    if (p->flightStatus != DEPARTING)
      p->drawLineFrom();
}

GLuint
Airport::__generateTip() const {
  Q_ASSERT(__data);

  QImage temp(MapWidget::GetSingleton().getAirportToolTipBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapWidget::GetSingleton().getAirportFont());
  painter.setPen(QColor(AIRPORTS_LABELS_FONT_COLOR));
  QRect rectangle(0, 0, 48, 12); // size of the tooltip.png
  painter.drawText(rectangle, Qt::AlignCenter, static_cast< QString >(__data->icao));
  __labelTip = MapWidget::loadImage(temp);
  return __labelTip;
}

