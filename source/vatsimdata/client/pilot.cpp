/*
    pilot.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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
#include <QtOpenGL>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "storage/settingsmanager.h"

#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "pilot.h"
#include "defines.h"

// how far from the airport the pilot must be to be recognized as "departing"
// or "arrived"
static const qreal PILOT_TO_AIRPORT = 0.1;

/*
 * 0 callsign
 * 1 cid
 * 2 realname
 * 3 clienttype
 * 4 frequency
 * 5 latitude
 * 6 longitude
 * 7 altitude
 * 8 groundspeed
 * 9 planned_aircraft
 * 10 planned_tascruise
 * 11 planned_depairport
 * 12 planned_altitude
 * 13 planned_destairport
 * 14 server
 * 15 protrevision
 * 16 rating
 * 17 transponder
 * 18 facilitytype
 * 19 visualrange
 * 20 planned_revision
 * 21 planned_flighttype
 * 22 planned_deptime
 * 23 planned_actdeptime
 * 24 planned_hrsenroute
 * 25 planned_minenroute
 * 26 planned_hrsfuel
 * 27 planned_minfuel
 * 28 planned_altairport
 * 29 planned_remarks
 * 30 planned_route
 * 31 planned_depairport_lat
 * 32 planned_depairport_lon
 * 33 planned_destairport_lat
 * 34 planned_destairport_lon
 * 35 atis_message
 * 36 time_last_atis_received
 * 37 time_logon
 * 38 heading
 * 39 QNH_iHg
 * 40 QNH_Mb
 */
Pilot::Pilot(const QStringList& _data, bool _prefiled) :
    Client(_data),
    __altitude(_data[7].toInt()),
    __groundSpeed(_data[8].toInt()),
    __squawk(_data[17]),
    __aircraft(_data[9]),
    __tas(_data[10].toInt()),
    __flightRules(_data[21] == "I" ? IFR : VFR),
    __remarks(_data[29]),
    __heading(_data[38].toUInt()),
    __pressure({_data[39], _data[40]}),
    __route({_data[11].toUpper(), _data[13].toUpper(), _data[30], _data[12].toUpper()}),
    __prefiledOnly(_prefiled) {
  // vatsim sometimes skips the 0 on the beginning
  if (__squawk.length() == 3)
    __squawk.prepend("0");

  __updateAirports();
  __setMyStatus();
//   __generateLines();
}

Pilot::~Pilot() {}

Client::Type
Pilot::clientType() const {
  return Client::Pilot;
}

void Pilot::__updateAirports() {
  if (!__route.origin.isEmpty()) {
    ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(__route.origin);
    ap->addOutbound(this);

    if (__prefiledOnly && ap->data()) {
      __position.latitude = ap->data()->latitude;
      __position.longitude = ap->data()->longitude;
    }

    if (ap->firs()[0])
      ap->firs()[0]->addFlight(this);

    if (ap->firs()[1])
      ap->firs()[1]->addFlight(this);
  }

  if (!__route.destination.isEmpty()) {
    ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(__route.destination);
    ap->addInbound(this);

    if (ap->firs()[0])
      ap->firs()[0]->addFlight(this);

    if (ap->firs()[1])
      ap->firs()[1]->addFlight(this);
  }
}

void
Pilot::__setMyStatus() {
  if (!__route.origin.isEmpty() && !__route.destination.isEmpty()) {
    const AirportRecord* ap_origin =
      VatsimDataHandler::getSingleton().activeAirports()[__route.origin]->data();
    const AirportRecord* ap_arrival =
      VatsimDataHandler::getSingleton().activeAirports()[__route.destination]->data();

    if ((ap_origin == ap_arrival) && (ap_origin != NULL)) // traffic pattern?
      if (__groundSpeed < 50) {
        __flightStatus = DEPARTING;
        return;
      }

    if (ap_origin)
      if ((VatsimDataHandler::distance(ap_origin->longitude, ap_origin->latitude,
                                           __position.longitude, __position.latitude) < PILOT_TO_AIRPORT) &&
      (__groundSpeed < 50)) {
        __flightStatus = DEPARTING;
        return;
      }

    if (ap_arrival)
      if ((VatsimDataHandler::distance(ap_arrival->longitude, ap_arrival->latitude,
                                           __position.longitude, __position.latitude) < PILOT_TO_AIRPORT) &&
      (__groundSpeed < 50)) {
        __flightStatus = ARRIVED;
        return;
      }
  } else { // no flight plan
    if (__groundSpeed > 50) {
      __flightStatus = AIRBORNE;
      return;
    }

    const AirportRecord* closest = NULL;
    qreal distance = 0.0;

    for (const AirportRecord & ap: AirportDatabase::getSingleton().airports()) {
      qreal temp = VatsimDataHandler::distance(ap.longitude, ap.latitude,
                   __position.longitude, __position.latitude);

      if (((temp < distance) && closest) || !closest) {
        closest = &ap;
        distance = temp;
      }
    }

    if (closest) {
      if (distance > PILOT_TO_AIRPORT) {
        __flightStatus = AIRBORNE;
        return;
      }

      __route.origin = closest->icao;
      ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(__route.origin);
      ap->addOutbound(this);
      __flightStatus = DEPARTING;
      return;
    }
  }

  __flightStatus = AIRBORNE;
}
