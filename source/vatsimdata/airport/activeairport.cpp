/*
    activeairport.cpp
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

#include "vatsimdata/fir.h"

#include "activeairport.h"
#include "defines.h"

ActiveAirport::ActiveAirport (QString _icao) :
    Airport(std::move(_icao)),
    __staff(new ControllerTableModel()),
    __inbounds(new FlightTableModel()),
    __outbounds(new FlightTableModel()) {
  if (__firs[0])
    __firs[0]->addAirport(this);
  if (__firs[1])
    __firs[1]->addAirport(this);
}

ActiveAirport::~ActiveAirport() {
  delete __staff;
  delete __inbounds;
  delete __outbounds;
}

unsigned
ActiveAirport::countDepartures() const {
  unsigned i = 0;
  
  for (const Pilot* p: __outbounds->flights())
    if (p->flightStatus() == Pilot::DEPARTING)
      i += 1;
  
  return i;
}

unsigned
ActiveAirport::countOutbounds() const {
  return __outbounds->rowCount();
}

unsigned
ActiveAirport::countArrivals() const {
  unsigned i = 0;
  
  for (const Pilot* p: __inbounds->flights())
    if (p->flightStatus() == Pilot::ARRIVED)
      i += 1;
  
  return i;
}

unsigned
ActiveAirport::countInbounds() const {
  return __inbounds->rowCount();
}

bool
ActiveAirport::hasApproach() const {
  for (const Controller* c: __staff->staff())
    if (c->facility() == Controller::APP)
      return true;
  
  return false;
}

Controller::Facilities
ActiveAirport::facilities() const {
  /* TODO Return a reference or something */
  
  Controller::Facilities facilities = 0;
  
  for (const Controller* c: __staff->staff())
    facilities |= c->facility();
  
  return facilities;
}

void
ActiveAirport::addStaff(const Controller* _c) {
  __staff->addStaff(_c);
}

void
ActiveAirport::addInbound(const Pilot* _p) {
  __inbounds->addFlight(_p);
}

void
ActiveAirport::addOutbound(const Pilot* _p) {
  __outbounds->addFlight(_p);
}


