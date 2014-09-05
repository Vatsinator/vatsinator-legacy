/*
    pilot.cpp
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
#include "storage/settingsmanager.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "pilot.h"

// how far from the airport the pilot must be to be recognized as "departing"
// or "arrived"
static constexpr qreal PilotToAirport = 0.15;

namespace {
  
  /**
   * Checks whether the given line crosses the IDL or the Greenwich Meridian.
   * For reference:
   * https://github.com/Vatsinator/Vatsinator/pull/2
   * Code by @Ucchi98.
   */
  bool crossesIdl(const QVector<LonLat>& _line) {
    LonLat p = _line.first(), c;
    for (int i = 1; i < _line.size(); ++i) {
      c = _line[i];
      qreal pSign = p.longitude() / qFabs(p.longitude());
      qreal cSign = c.longitude() / qFabs(c.longitude());
      
      if (pSign != cSign) {
        qreal dst1 = VatsimDataHandler::fastDistance(p, c), dst2;
        if (pSign < 0)
          dst2 = VatsimDataHandler::fastDistance(p.longitude() + 360.0, p.latitude(), c.longitude(), c.latitude());
        else
          dst2 = VatsimDataHandler::fastDistance(p.longitude(), p.latitude(), c.longitude() + 360.0, c.latitude());
        
        if (dst1 > dst2)
          return true;
      }
      
      p = c;
    }
    
    return false;
  }

}

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
    __oldPosition(Client::position()),
    __tas(_data[10].toInt()),
    __flightRules(_data[21] == "I" ? Ifr : Vfr),
    __std(QTime::fromString(_data[22], "hhmm")),
    __atd(QTime::fromString(_data[23], "hhmm")),
    __progress(-1),
    __remarks(_data[29]),
    __heading(_data[38].toUInt()),
    __pressure({_data[39], _data[40]}),
    __route({_data[11].toUpper(), _data[13].toUpper(), _data[30], _data[12].toUpper()}),
    __origin(nullptr),
    __destination(nullptr),
    __prefiledOnly(_prefiled) {
    
  // vatsim sometimes skips the 0 on the beginning
  if (__squawk.length() == 3)
    __squawk.prepend("0");
  
  if (__std.isValid() && __std != QTime(0, 0)) {
    __sta = QTime(__std.hour() + _data[24].toInt(), __std.minute() + _data[25].toInt());
  }
  
  __updateAirports();
  __fixupRoute();
  __discoverFlightPhase();
}

Pilot::~Pilot() {}

void
Pilot::update(const QStringList& _data) {
  __oldPosition = position();
  Client::update(_data);
  
  __prefiledOnly = false;
  __altitude = _data[7].toInt();
  __groundSpeed = _data[8].toInt();
  __squawk = _data[17];
  __aircraft = _data[9];
  __tas = _data[10].toInt();
  __flightRules = _data[21] == "I" ? Ifr : Vfr;
  __std = QTime::fromString(_data[22], "hhmm");
  __atd = QTime::fromString(_data[23], "hhmm");
  __remarks = _data[29];
  __heading = _data[38].toUInt();
  __pressure = Pressure{ _data[39], _data[40] };
  
  __discoverFlightPhase();
  
  // update airports if anything has changed
  QString tOrigin(_data[11].toUpper());
  QString tDestination(_data[13].toUpper());
  if (
    !origin()      || origin()->icao() != tOrigin ||
    !destination() || destination()->icao() != tDestination
  ) {
    __route = Route{ tOrigin, tDestination, _data[30], _data[12].toUpper() };
    __updateAirports();
    __fixupRoute();
  } else if (oldPosition() != position()) {
    // update just the position
    for (LonLat& p: __route.waypoints)
      if (p == oldPosition())
        p = position();
  }
  
  if (__squawk.length() == 3)
    __squawk.prepend("0");
  
  if (__std.isValid() && __sta != QTime(0, 0))
    __sta = QTime(__std.hour() + _data[24].toInt(), __std.minute() + _data[25].toInt());
  
  // invalidate eta
  __eta = QTime();
  
  // invalidate progress
  __progress = -1;
  
  emit updated();
}

const QTime &
Pilot::eta() const {
  if (__phase == Departing) {
    __eta = QTime();
    return __eta;
  }
  
  if (__eta.isValid())
    return __eta;
  
  const Airport* to = vApp()->vatsimDataHandler()->findAirport(__route.destination);
  if (to) {
    // calculate distance between pilot and destination airport
    qreal dist = VatsimDataHandler::nmDistance(
        qDegreesToRadians(position().latitude()),
        qDegreesToRadians(position().longitude()),
        qDegreesToRadians(to->data()->latitude),
        qDegreesToRadians(to->data()->longitude)
      );
    
    int secs = (dist / static_cast<qreal>(groundSpeed())) * 60.0 * 60.0;
    
    __eta = QDateTime::currentDateTimeUtc().time().addSecs(secs);
  } else {
    __eta = __sta;
  }
  
  return __eta;
}

int
Pilot::progress() const {
  if (__phase == Arrived)
    return 100;
  else if (__phase == Departing)
    return 0;
  
  if (__progress == -1) {
    const Airport* from = vApp()->vatsimDataHandler()->findAirport(__route.origin);
    const Airport* to = vApp()->vatsimDataHandler()->findAirport(__route.destination);
    
    if (from && to) {
      qreal total = VatsimDataHandler::nmDistance(
        qDegreesToRadians(from->data()->latitude),
        qDegreesToRadians(from->data()->longitude),
        qDegreesToRadians(to->data()->latitude),
        qDegreesToRadians(to->data()->longitude)
      );
      
      qreal left = VatsimDataHandler::nmDistance(
        qDegreesToRadians(position().latitude()),
        qDegreesToRadians(position().longitude()),
        qDegreesToRadians(to->data()->latitude),
        qDegreesToRadians(to->data()->longitude)
      );
      
      __progress = 100 - (100 * left / total);
    } else {
      __progress = 0;
    }
  }
  
  return __progress;
}

void Pilot::__updateAirports() {
  if (!__route.origin.isEmpty()) {
    Airport* ap = vApp()->vatsimDataHandler()->findAirport(__route.origin);
    if (ap) {
      ap->addOutbound(this);
      __origin = ap;
      
      if (__prefiledOnly || !hasValidPosition()) {
        setPosition(LonLat(ap->data()->longitude, ap->data()->latitude));
      } else {
        __route.waypoints << LonLat(ap->data()->longitude, ap->data()->latitude);
      }
    }
  }
  
  __route.waypoints << position();
  
  if (!__route.destination.isEmpty()) {
    Airport* ap = vApp()->vatsimDataHandler()->findAirport(__route.destination);
    if (ap) {
      ap->addInbound(this);
      __destination = ap;
      
      if (!__prefiledOnly)
        __route.waypoints << LonLat(ap->data()->longitude, ap->data()->latitude);
    }
  }
  
  
}

void
Pilot::__discoverFlightPhase() {
  if (!__route.origin.isEmpty() && !__route.destination.isEmpty()) {
    if ((origin() == destination()) && (origin() != nullptr)) // traffic pattern?
      if (__groundSpeed < 50) {
        __phase = Departing;
        return;
      }
    
    if (origin())
      if ((VatsimDataHandler::fastDistance(origin()->data()->longitude, origin()->data()->latitude,
                                           position().longitude(), position().latitude()) < PilotToAirport) &&
      (__groundSpeed < 50)) {
        __phase = Departing;
        return;
      }

    if (destination())
      if ((VatsimDataHandler::fastDistance(destination()->data()->longitude, destination()->data()->latitude,
                                           position().longitude(), position().latitude()) < PilotToAirport) &&
      (__groundSpeed < 50)) {
        __phase = Arrived;
        return;
      }
  } else { // no flight plan
    if (__groundSpeed > 50) {
      __phase = Airborne;
      return;
    }

    const AirportRecord* closest = nullptr;
    qreal distance = 0.0;
    
    for (const AirportRecord& ap: AirportDatabase::getSingleton().airports()) {
      qreal temp = VatsimDataHandler::fastDistance(ap.longitude, ap.latitude,
                             position().longitude(), position().latitude());

      if (((temp < distance) && closest) || !closest) {
        closest = &ap;
        distance = temp;
      }
    }
    
    if (closest) {
      if (distance > PilotToAirport) {
        __phase = Airborne;
        return;
      }
    
      __route.origin = QString(closest->icao);
      Airport* ap = vApp()->vatsimDataHandler()->findAirport(__route.origin);
      __origin = ap;
      
      if (ap) {
        ap->addOutbound(this);
      }
      
      __phase = Departing;
      return;
    }
  }

  __phase = Airborne;
}

void
Pilot::__fixupRoute() {
  if (crossesIdl(__route.waypoints)) {
    for (LonLat& p: __route.waypoints)
      if (p.longitude() < 0)
        p.rx() += 360.0;
  }
}
