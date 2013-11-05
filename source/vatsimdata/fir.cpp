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

#include "vatsimdata/models/airporttablemodel.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "fir.h"
#include "defines.h"

Fir::Fir() :
    __staff(new ControllerTableModel()),
    __flights(new FlightTableModel()),
    __airports(new AirportTableModel()),
    __uirStaffCount(0) {}

Fir::~Fir() {
  delete __staff;
  delete __flights;
  delete __airports;
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
Fir::loadHeader(const FirHeader& _header) {
  __icao = _header.icao;
  __oceanic = static_cast<bool>(_header.oceanic);
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
  return !__staff->staff().isEmpty() && __uirStaffCount < static_cast<unsigned>(__staff->rowCount());
}
