/*
    airport.cpp
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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "airport.h"

Airport::Airport(const QString& icao) :
    __data(vApp()->airportDatabase()->find(icao)),
    __icao(icao),
    __staff(new ControllerTableModel()),
    __inbounds(new FlightTableModel()),
    __outbounds(new FlightTableModel()) {
  
  Q_ASSERT(__data);
}

Airport::Airport(const AirportRecord* record) :
    __data(record),
    __icao(__data->icao),
    __staff(new ControllerTableModel()),
    __inbounds(new FlightTableModel()),
    __outbounds(new FlightTableModel()) {
  
  Q_ASSERT(__data);
}

Airport::~Airport() {
  delete __staff;
  delete __inbounds;
  delete __outbounds;
}

unsigned
Airport::countDepartures(bool includePrefiled) const {
  unsigned i = 0;
  
  for (const Pilot* p: __outbounds->flights()) {
    if (p->phase() == Pilot::Departing) {
      if (!p->isPrefiledOnly() || (p->isPrefiledOnly() && includePrefiled))
        i += 1;
    }
  }
  
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
    if (p->phase() == Pilot::Arrived)
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
Airport::addStaff(const Controller* atc) {
  __staff->add(atc);
  connect(atc,  SIGNAL(updated()),
          this, SIGNAL(updated()));
  connect(atc,  SIGNAL(destroyed(QObject*)),
          this, SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
}

void
Airport::addInbound(const Pilot* pilot) {
  __inbounds->add(pilot);
  connect(pilot,        SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(pilot,        SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
}

void
Airport::addOutbound(const Pilot* pilot) {
  __outbounds->add(pilot);
  connect(pilot,        SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(pilot,        SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
}

bool
Airport::isEmpty() const {
  return
    __staff->rowCount() == 0 &&
    __inbounds->rowCount() == 0 &&
    __outbounds->rowCount() == 0;
}

bool
Airport::isStaffed() const {
  return
    __staff->rowCount() > 0 || 
    __inbounds->rowCount() > 0 ||
    __outbounds->rowCount() > 0;
}

LonLat
Airport::position() const {
  return qMove(LonLat(data()->longitude, data()->latitude));
}
