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

#include <QtGui>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "storage/settingsmanager.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "pilot.h"
#include "defines.h"

// how far from the airport the pilot must be to be recognized as "departing"
// or "arrived"
static constexpr qreal PilotToAirport = 0.1;

namespace {

  inline qreal deg2Rad(qreal deg) {
    static constexpr qreal PI = 3.14159265359;
    return deg * PI / 180;
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
  
  if (__std.isValid() && __sta != QTime(0, 0)) {
    __sta = QTime(__std.hour() + _data[24].toInt(), __std.minute() + _data[25].toInt());
  }
  
  __updateAirports();
  __setMyStatus();
}

Pilot::~Pilot() {}

void
Pilot::update(const QStringList& _data) {
  Client::update(_data);
  
  __altitude = _data[7].toInt();
  __groundSpeed = _data[8].toInt();
  __squawk = _data[17].toInt();
  __aircraft = _data[9];
  __tas = _data[10].toInt();
  __flightRules = _data[21] == "I" ? Ifr : Vfr;
  __std = QTime::fromString(_data[22], "hhmm");
  __atd = QTime::fromString(_data[23], "hhmm");
  __remarks = _data[29];
  __heading = _data[38].toUInt();
  __pressure = Pressure{ _data[39], _data[40] };
  __route = Route{ _data[11].toUpper(), _data[13].toUpper(), _data[30], _data[12].toUpper() };
  
  // update airports if anything changed
  if (
    !origin()      || origin()->icao() != __route.origin        ||
    !destination() || destination()->icao() != __route.origin
  )
    __updateAirports();
  
  if (__squawk.length() == 3)
    __squawk.prepend("0");
  
  if (__std.isValid() && __sta != QTime(0, 0))
    __sta = QTime(__std.hour() + _data[24].toInt(), __std.minute() + _data[25].toInt());
  
  // invalidate eta
  __eta = QTime();
  
  // invalidate progress
  __progress = -1;
}

const QTime &
Pilot::eta() const {
  if (__flightStatus == Departing) {
    __eta = QTime();
    return __eta;
  }
  
  if (__eta.isValid())
    return __eta;
  
  const Airport* to = VatsimDataHandler::getSingleton().findAirport(__route.destination);
  if (to) {
    // calculate distance between pilot and destination airport
    qreal dist = VatsimDataHandler::nmDistance(
        deg2Rad(position().latitude()),
        deg2Rad(position().longitude()),
        deg2Rad(to->data()->latitude),
        deg2Rad(to->data()->longitude)
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
  if (__flightStatus == Arrived)
    return 100;
  else if (__flightStatus == Departing)
    return 0;
  
  if (__progress == -1) {
    const Airport* from = VatsimDataHandler::getSingleton().findAirport(__route.origin);
    const Airport* to = VatsimDataHandler::getSingleton().findAirport(__route.destination);
    
    if (from && to) {
      qreal total = VatsimDataHandler::nmDistance(
        deg2Rad(from->data()->latitude),
        deg2Rad(from->data()->longitude),
        deg2Rad(to->data()->latitude),
        deg2Rad(to->data()->longitude)
      );
      
      qreal left = VatsimDataHandler::nmDistance(
        deg2Rad(position().latitude()),
        deg2Rad(position().longitude()),
        deg2Rad(to->data()->latitude),
        deg2Rad(to->data()->longitude)
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
    Airport* ap = VatsimDataHandler::getSingleton().findAirport(__route.origin);
    if (ap) {
      ap->addOutbound(this);
      __origin = ap;
      
      if (__prefiledOnly) {
        setPosition(LonLat(ap->data()->longitude, ap->data()->latitude));
      } else {
        __route.waypoints << LonLat(ap->data()->longitude, ap->data()->latitude);
      }
    }
  }
  
  __route.waypoints << position();
  
  if (!__route.destination.isEmpty()) {
    Airport* ap = VatsimDataHandler::getSingleton().findAirport(__route.destination);
    if (ap) {
      ap->addInbound(this);
      __destination = ap;
      
      if (!__prefiledOnly)
        __route.waypoints << LonLat(ap->data()->longitude, ap->data()->latitude);
    }
  }
}

void
Pilot::__setMyStatus() {
  if (!__route.origin.isEmpty() && !__route.destination.isEmpty()) {
    if ((origin() == destination()) && (origin() != nullptr)) // traffic pattern?
      if (__groundSpeed < 50) {
        __flightStatus = Departing;
        return;
      }
    
    if (origin())
      if ((VatsimDataHandler::fastDistance(origin()->data()->longitude, origin()->data()->latitude,
                                           position().longitude(), position().latitude()) < PilotToAirport) &&
      (__groundSpeed < 50)) {
        __flightStatus = Departing;
        return;
      }

    if (destination())
      if ((VatsimDataHandler::fastDistance(destination()->data()->longitude, destination()->data()->latitude,
                                           position().longitude(), position().latitude()) < PilotToAirport) &&
      (__groundSpeed < 50)) {
        __flightStatus = Arrived;
        return;
      }
  } else { // no flight plan
    if (__groundSpeed > 50) {
      __flightStatus = Airborne;
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
        __flightStatus = Airborne;
        return;
      }
    
      __route.origin = QString(closest->icao);
      Airport* ap = VatsimDataHandler::getSingleton().findAirport(__route.origin);
      __origin = ap;
      
      if (ap) {
        ap->addOutbound(this);
      }
      
      __flightStatus = Departing;
      return;
    }
  }

  __flightStatus = Airborne;
}
 
// bool Pilot::__isCrossingIDL(QVector<GLfloat>& line) const
// {
//   bool isCrossingIDL = false;
// 
//   GLfloat plon = line[0];
//   GLfloat plat = line[1];
//   GLfloat clon = plon;
//   GLfloat clat = plat;
// 
//   for(int i=2 ; i<line.size() ; i++){
// 		clon = line[i]; i++;
// 		clat = line[i];
// 
// 		double pSign = plon / fabs(plon);
// 		double cSign = clon / fabs(clon);
// 		double dst1 = 0;
// 		double dst2 = 0;
// 
// 		// crossing the IDL or the Greenwich Meridian
// 		if(pSign!=cSign){
// 
//              		dst1 = VatsimDataHandler::distance(plon, plat, clon, clat);
// 			if(pSign<0)
//     				dst2 = VatsimDataHandler::distance(plon + 360, plat, clon, clat);
// 			else	
//              			dst2 = VatsimDataHandler::distance(plon, plat, clon + 360, clat);
// 
// 			if(dst1>dst2) isCrossingIDL = true;
// 
// 			/* debug print
// 			printf("%s: %f %f -> %f %f, Dst1: %f Dst2: %f, IDL: %s\n",
// 				__callsign.toLatin1().data(),
// 				plon, plat, clon, clat, dst1, dst2,
// 				isCrossingIDL ? "crossing IDL" : "crossing meridian");
//                         */
// 		}
// 		if(isCrossingIDL) return true;
// 
// 		plon = clon;
// 		plat = clat;
//   }
//   return false;
// }
