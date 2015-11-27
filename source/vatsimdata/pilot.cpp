/*
    pilot.cpp
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
Pilot::Pilot(const QStringList& data, bool prefiled) :
    Client(data),
    __altitude(data[7].toInt()),
    __groundSpeed(data[8].toInt()),
    __squawk(data[17]),
    __aircraft(data[9]),
    __oldPosition(Client::position()),
    __tas(data[10].toInt()),
    __flightRules(data[21] == "I" ? Ifr : Vfr),
    __std(QTime::fromString(data[22], "hhmm")),
    __atd(QTime::fromString(data[23], "hhmm")),
    __progress(-1),
    __remarks(data[29]),
    __heading(data[38].toUInt()),
    __pressure({ data[39], data[40] }),
    __route({data[11].toUpper(), data[13].toUpper(), data[30], data[12].toUpper(), {}}),
    __origin(nullptr),
    __destination(nullptr),
    __prefiledOnly(prefiled)
{
    // Vatsim sometimes skips the 0 on the beginning
    if (__squawk.length() == 3)
        __squawk.prepend("0");
    
    if (__std.isValid() && __std != QTime(0, 0)) {
        // STA = STD + flight duration
        __sta = QTime(__std.hour() + data[24].toInt(), __std.minute() + data[25].toInt());
    }
    
    connect(this, &Pilot::updated, this, &Pilot::__updateWaypoints);
    connect(this, &Pilot::updated, this, &Pilot::__invalidateProgress);
    
    __updateAirports();
    Q_ASSERT(origin());
    Q_ASSERT(destination());
    __phase = __discoverFlightPhase();
    __updateWaypoints();
}

void
Pilot::update(const QStringList& data)
{
    __oldPosition = position();
    Client::update(data);
    
    if (__prefiledOnly) {
        __prefiledOnly = false;
        emit wentOnline();
    }
    
    int alt = data[7].toInt();
    if (__altitude != alt) {
        __altitude = alt;
        emit altitudeChanged(__altitude);
    }
    
    int gs = data[8].toInt();
    if (__groundSpeed != gs) {
        __groundSpeed = gs;
        emit groundSpeedChanged(__groundSpeed);
    }
    
    QString squawk = data[17];
    if (squawk.length() == 3)
        squawk.prepend("0");
    if (__squawk != squawk) {
        __squawk = squawk;
        emit squawkChanged(__squawk);
    }
    
    if (__aircraft != data[9]) {
        __aircraft = data[9];
        emit aircraftChanged(__aircraft);
    }
    
    int tas = data[10].toInt();
    if (tas != __tas) {
        __tas = tas;
        emit tasChanged(__tas);
    }
    
    FlightRules flightRules = data[21] == "I" ? Ifr : Vfr;
    if (__flightRules != flightRules) {
        __flightRules = flightRules;
        emit flightRulesChanged(__flightRules);
    }
    
    QTime std = QTime::fromString(data[22], "hhmm");
    if (std != __std) {
        __std = QTime::fromString(data[22], "hhmm");
        emit stdChanged(__std);
        
        if (__std.isValid() && __std != QTime(0, 0)) {
            // STA = STD + flight duration
            __sta = QTime(__std.hour() + data[24].toInt(), __std.minute() + data[25].toInt());
        }
    }
    
    QTime atd = QTime::fromString(data[23], "hhmm");
    if (atd != __atd) {
        __atd = QTime::fromString(data[23], "hhmm");
        emit atdChanged(__atd);
    }
    
    if (__remarks != data[29]) {
        __remarks = data[29];
        emit remarksChanged(__remarks);
    }
    
    unsigned heading = data[38].toUInt();
    if (__heading != heading) {
        __heading = heading;
        emit headingChanged(__heading);
    }
    
    __pressure = Pressure{ data[39], data[40] };
    
    // update airports if anything has changed
    QString tOrigin(data[11].toUpper());
    QString tDestination(data[13].toUpper());
    
    if (origin()->icao() != tOrigin || destination()->icao() != tDestination) {
        __route = Route{ tOrigin, tDestination, data[30], data[12].toUpper(), {} };
        __updateAirports();
        emit airportsUpdated();
    }
    
    Phase phase = __discoverFlightPhase();
    if (__phase != phase) {
        __phase = phase;
        emit phaseChanged(__phase);
    }
    
    emit updated();
}

const QTime&
Pilot::eta() const
{
    if (__phase == Departing) {
        __eta = QTime();
        return __eta;
    }
    
    if (__eta.isValid())
        return __eta;

    if (destination()->isValid()) {
        // calculate distance between pilot and destination airport
        qreal dist = VatsimDataHandler::nmDistance(position(), destination()->position());
        int secs = (dist / static_cast<qreal>(groundSpeed())) * 60.0 * 60.0;
        __eta = QDateTime::currentDateTimeUtc().time().addSecs(secs);
    } else
        __eta = __sta;
        
    return __eta;
}

int
Pilot::progress() const
{
    if (__phase == Arrived)
        return 100;
    else if (__phase == Departing)
        return 0;
        
    if (__progress == -1) {
        if (origin()->isValid() && destination()->isValid()) {
            qreal total = VatsimDataHandler::nmDistance(origin()->position(), destination()->position());
            qreal left = VatsimDataHandler::nmDistance(position(), destination()->position());
            __progress = 100 - (100 * left / total);
        } else
            __progress = 0;
    }
    
    return __progress;
}

void
Pilot::__updateAirports()
{
    __findOrigin();    
    __findDestination();
}

void
Pilot::__findOrigin()
{
    if (!__route.origin.isEmpty()) {
        Airport* ap = vApp()->vatsimDataHandler()->findAirport(__route.origin);
        if (ap) {
            ap->addOutbound(this);
            __origin = ap;
            
            if (__prefiledOnly || !hasValidPosition())
                setPosition(ap->position());
        } else {
            __origin = new Airport(__route.origin, this);
        }
    } else { // no origin airport specified - no flight plan filed yet
        if (__prefiledOnly || !hasValidPosition()  || __maybeAirborne()) {
            __origin = new Airport(this);
        } else {
            /* TODO Use Spatial for this */
            const AirportRecord* closest = nullptr;
            qreal distance = 0.0;
            
            for (const AirportRecord& ap : vApp()->airportDatabase()->airports()) {
                qreal temp = VatsimDataHandler::fastDistance(ap.longitude, ap.latitude,
                    position().longitude(), position().latitude());
                
                if (((temp < distance) && closest) || !closest) {
                    closest = &ap;
                    distance = temp;
                }
            }
            
            if (closest && distance < PilotToAirport) {
                __route.origin = QString(closest->icao);
                __findOrigin();
            } else {
                __origin = new Airport();
            }
        }
    }
}

void
Pilot::__findDestination()
{
    if (!__route.destination.isEmpty()) {
        Airport* ap = vApp()->vatsimDataHandler()->findAirport(__route.destination);
        if (ap) {
            ap->addInbound(this);
            __destination = ap;
        } else {
            __destination = new Airport(__route.destination, this);
        }
    } else {
        __destination = new Airport(this);
    }
}

Pilot::Phase
Pilot::__discoverFlightPhase()
{
    if (origin()->icao() == destination()->icao()) { // traffic pattern
        if (__maybeAirborne()) {
            return Airborne;
        } else {
            if (__phase == Airborne)
                return Arrived;
            else
                return Departing;
        }
    }
    
    if (origin()->isValid()) {
        if ((VatsimDataHandler::fastDistance(origin()->position(), position()) < PilotToAirport) && (!__maybeAirborne())) {
            return Departing;
        }
    }
    
    if (destination()->isValid()) {
        if ((VatsimDataHandler::fastDistance(destination()->position(), position()) < PilotToAirport) && (!__maybeAirborne())) {
            return Arrived;
        }
    }
    
    return Airborne;
}

bool
Pilot::__maybeAirborne()
{
    return groundSpeed() > 50;
}

void
Pilot::__updateWaypoints()
{
    __route.waypoints.clear();
    
    if (origin()->isValid())
        __route.waypoints << origin()->position();
    
    __route.waypoints << position();
    
    if (!isPrefiledOnly() && destination()->isValid())
        __route.waypoints << destination()->position();
}

void
Pilot::__invalidateProgress()
{
    // invalidate eta
    __eta = QTime();
    
    // invalidate progress
    __progress = -1;
    
    emit progressChanged();
}
