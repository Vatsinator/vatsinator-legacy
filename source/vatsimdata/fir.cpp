/*
    fir.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "db/firdatabase.h"
#include "vatsimdata/models/airporttablemodel.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "fir.h"

Fir::Fir(const FirRecord* _data) :
     __data(_data),
     __icao(QString::fromUtf8(_data->header.icao)),
     __oceanic(_data->header.oceanic),
     __staff(new ControllerTableModel(this)),
     __uirStaff(new ControllerTableModel(this)),
     __flights(new FlightTableModel(this)),
     __airports(new AirportTableModel(this)) {
  
  Q_ASSERT(__data);
}

Fir::~Fir() {}

void
Fir::addStaff(const Controller* _c) {
  __staff->add(_c);
  connect(_c,           SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(_c,           SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
}

void
Fir::addUirStaff(const Controller* _c) {
  __uirStaff->add(_c);
  connect(_c,           SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(_c,           SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
}

void
Fir::addFlight(const Pilot* _p) {
  __flights->add(_p);
  connect(_p,           SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
}

void
Fir::addAirport(const Airport* _ap) {
  __airports->addAirport(_ap);
}

void
Fir::fixupName() {
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

bool
Fir::isStaffed() const {
  return __staff->rowCount() > 0;
}

bool
Fir::isEmpty() const {
  return __staff->rowCount() == 0;
}

bool
Fir::hasValidPosition() const {
  return data()->header.textPosition.x != .0f && data()->header.textPosition.y != .0f;
}

void
Fir::setName(const QString& _n) {
  __name = _n;
}

void
Fir::setCountry(const QString& _c) {
  __country = _c;
}
