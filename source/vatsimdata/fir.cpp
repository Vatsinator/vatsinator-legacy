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

#include <QtGui>

#include "db/firdatabase.h"
#include "vatsimdata/models/airporttablemodel.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "fir.h"
#include "defines.h"

Fir::Fir(const FirRecord* _data) :
     __data(_data),
     __icao(QString::fromUtf8(_data->header.icao)),
     __oceanic(_data->header.oceanic),
     __staff(new ControllerTableModel()),
     __flights(new FlightTableModel()),
     __airports(new AirportTableModel()) {
  
  Q_ASSERT(__data);
}

Fir::~Fir() {
  delete __staff;
  delete __flights;
  delete __airports;
}

void
Fir::addStaff(const Controller* _c) {
//   __staff->addStaff(_c);
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
