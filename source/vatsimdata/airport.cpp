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

#include <algorithm>
#include <QtCore>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "airport.h"

Airport::Airport(const AirportRecord* record) :
    __data(record),
    __icao(__data->icao),
    __staff(new AtcTableModel(this)),
    __inbounds(new FlightTableModel(this)),
    __outbounds(new FlightTableModel(this)) {
  
  Q_ASSERT(__data);
  
  auto fillFir = [this](const QString& icao, bool isFss) {
    Fir* f = vApp()->vatsimDataHandler()->findFir(icao, isFss);
    if (f) {
      f->addAirport(this);
      __firs << f;
    }
  };
  
  fillFir(QString(__data->fir_a), __data->is_fir_a_oceanic);
  fillFir(QString(__data->fir_b), __data->is_fir_b_oceanic);
}

Airport::~Airport() {}

unsigned
Airport::countDepartures(bool includePrefiled) const {
  return std::count_if(__outbounds->flights().begin(), __outbounds->flights().end(), 
                       [includePrefiled](const Pilot* p) {
                         return p->phase() == Pilot::Departing &&
                          (!p->isPrefiledOnly() || (p->isPrefiledOnly() && includePrefiled));
                       });
}

unsigned
Airport::countOutbounds() const {
  return __outbounds->rowCount();
}

unsigned
Airport::countArrivals() const {
  return std::count_if(__inbounds->flights().begin(), __inbounds->flights().end(),
                       [](const Pilot* p) {
                         return p->phase() == Pilot::Arrived;
                       });
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
  
  for (Fir* f: __firs)
    f->addFlight(pilot);
  
  emit updated();
}

void
Airport::addOutbound(const Pilot* pilot) {
  __outbounds->add(pilot);
  connect(pilot,        SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(pilot,        SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  
  for (Fir* f: __firs)
    f->addFlight(pilot);
  
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
