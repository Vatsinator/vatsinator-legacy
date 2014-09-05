/*
    client.cpp
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

#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "client.h"

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
Client::Client(const QStringList& _data) :
    __pid(_data[1].toUInt()),
    __callsign(_data[0]),
    __realName(_data[2].simplified()),
    __server(_data[14]),
    __onlineFrom(QDateTime::fromString(_data[37], "yyyyMMddhhmmss")),
    __position(_data[6].toFloat(), _data[5].toFloat()),
    __timestamp(vApp()->vatsimDataHandler()->currentTimestamp()) {}

Client::~Client() {}

void
Client::update(const QStringList& _data) {
  __pid = _data[1].toUInt();
  __realName = _data[2].simplified();
  __server = _data[14];
  __onlineFrom = QDateTime::fromString(_data[37], "yyyyMMddhhmmss");
  __position = LonLat(_data[6].toFloat(), _data[5].toFloat());
  
  __timestamp = vApp()->vatsimDataHandler()->currentTimestamp();
}

void
Client::invalidate() {
  emit invalid();
}

bool
Client::isOnline() const {
  return __timestamp == vApp()->vatsimDataHandler()->currentTimestamp();
}

bool
Client::hasValidPosition() const {
  return !position().isNull() && position().x() != 0.0 && position().y() != 0.0;
}

void
Client::setPosition(const LonLat& _position) {
  __position = _position;
}

