/*
    airport.cpp
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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "airport.h"
#include "defines.h"

Airport::Airport(const QString& _icao) :
    __firs(nullptr, nullptr),
    __data(AirportDatabase::getSingleton().find(_icao)),
    __staff(new ControllerTableModel()),
    __inbounds(new FlightTableModel()),
    __outbounds(new FlightTableModel()) {
  
  Q_CHECK_PTR(__data);
  
  __firs.first = FirDatabase::getSingleton().find(
    QString(__data->fir_a),
    __data->is_fir_a_oceanic
  );
  
  __firs.second = FirDatabase::getSingleton().find(
    QString(__data->fir_b),
    __data->is_fir_b_oceanic
  );
}

Airport::Airport(const AirportRecord* _ap) :
    __firs(nullptr, nullptr),
    __data(_ap),
    __staff(new ControllerTableModel()),
    __inbounds(new FlightTableModel()),
    __outbounds(new FlightTableModel()) {
  
  Q_CHECK_PTR(__data);
  
  __firs.first = FirDatabase::getSingleton().find(
    QString(__data->fir_a),
    __data->is_fir_a_oceanic
  );
  
  __firs.second = FirDatabase::getSingleton().find(
    QString(__data->fir_b),
    __data->is_fir_b_oceanic
  );
}

unsigned
Airport::countDepartures() const {
  unsigned i = 0;
  
  for (const Pilot* p: __outbounds->flights())
    if (p->flightStatus() == Pilot::Departing)
      i += 1;
  
  return i;
}

unsigned
Airport::countOutbounds() const {
  return __outbounds->rowCount();
}

unsigned
Airport::countArrivals() const {
  unsigned i = 0;
  
  for (const Pilot* p: __inbounds->flights())
    if (p->flightStatus() == Pilot::Arrived)
      i += 1;
  
  return i;
}

unsigned
Airport::countInbounds() const {
  return __inbounds->rowCount();
}

Controller::Facilities
Airport::facilities() const {
  Controller::Facilities facilities = 0;
  
  for (const Controller* c: __staff->staff())
    facilities |= c->facility();
  
  return facilities;
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

bool
Airport::isEmpty() const {
  return __staff->rowCount() == 0 && __inbounds->rowCount() == 0 && __outbounds->rowCount() == 0;
}