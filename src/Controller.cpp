/*
    Controller.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "../include/Controller.h"

#include "../include/AirportsDatabase.h"
#include "../include/FirsDatabase.h"
#include "../include/VatsimDataHandler.h"
#include "../include/defines.h"

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
Controller::Controller(const QStringList& _data) {
	callsign = _data[0];
	pid = _data[1].toUInt();
	realName = _data[2];
	
	frequency = _data[4];
	server = _data[14];
	rating = _data[16].toInt();
	atis = _data[35];
	
	if (atis[0] == '$') {
		auto index = atis.indexOf('^');
		atis.remove(0, index + 2);
	}
	atis.replace((QString)'^' + (char)167, "\n");
	
	onlineFrom = QDateTime::fromString(_data[37], "yyyyMMddhhmmss");
	
	__setMyIcaoAndFacility();
}

void
Controller::__setMyIcaoAndFacility() {
	QStringList sections = callsign.split('_');
	if (sections.back() == "CTR") {
		facility = CTR;
		airport = NULL;
		
		QString& icao = sections.front();
		
		Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(icao);
		if (fir)
			fir->addStaff(this);
		else {
			// handle USA three-letters callsigns
			if (icao.length() == 3) {
				fir = FirsDatabase::GetSingleton().findFirByIcao("K" + icao);
				if (fir) {
					fir->addStaff(this);
					return;
				}
			}
			
			for (const QString& alias: VatsimDataHandler::GetSingleton().getAliases().values(icao)) {
				fir = FirsDatabase::GetSingleton().findFirByIcao(alias);
				if (fir) {
					fir->addStaff(this);
					return;
				}
			}
			
			Uir* uir = VatsimDataHandler::GetSingleton().findUIR(icao);
			if (uir) {
				uir->addStaff(this);
				return;
			}
			
#ifndef NO_DEBUG
			qDebug() << "FIR not found: " << callsign;
#endif
			
		}
		
		return;
	} else if (sections.back() == "FSS") {
		facility = FSS;
		airport = NULL;
		
		QString& icao = sections.front();
		
		Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(icao, true);
		if (fir)
			fir->addStaff(this);
		else {
			Uir* uir = VatsimDataHandler::GetSingleton().findUIR(icao);
			if (uir) {
				uir->addStaff(this);
				return;
			}
			
			for (QString& alias: VatsimDataHandler::GetSingleton().getAliases().values(icao)) {
				fir = FirsDatabase::GetSingleton().findFirByIcao(alias + "F");
				if (fir) {
					fir->addStaff(this);
					return;
				}
			}
#ifndef NO_DEBUG
			qDebug() << "FIR not found: " << callsign;
#endif
		}
		
		return;
	} else if (
			sections.back() == "APP" ||
			sections.back() == "TWR" ||
			sections.back() == "GND" ||
			sections.back() == "DEL" ||
			sections.back() == "ATIS") {
		if (sections.back() == "APP")
			facility = APP;
		else if (sections.back() == "TWR")
			facility = TWR;
		else if (sections.back() == "GND")
			facility = GND;
		else if (sections.back() == "DEL")
			facility = DEL;
		else if (sections.back() == "ATIS")
			facility = ATIS;
		
		AirportRecord* apShot = AirportsDatabase::GetSingleton().find(sections.front());
		if (apShot) {
			AirportObject* ap = VatsimDataHandler::GetSingleton().addActiveAirport(sections.front());
			ap->addStaff(this);
			airport = ap->getData();
			return;
		} else {
			if (sections.front().length() == 3) {
				QString alias = "K" + sections.front();
				apShot = AirportsDatabase::GetSingleton().find(alias);
				if (apShot) {
					AirportObject* ap = VatsimDataHandler::GetSingleton().addActiveAirport(alias);
					ap->addStaff(this);
					airport = ap->getData();
					return;
				}
			}
			
			for (QString& alias: VatsimDataHandler::GetSingleton().getAliases().values(sections.front())) {
				apShot = AirportsDatabase::GetSingleton().find(alias);
				if (apShot) {
					AirportObject* ap = VatsimDataHandler::GetSingleton().addActiveAirport(alias);
					ap->addStaff(this);
					airport = ap->getData();
					return;
				}
			}
#ifndef NO_DEBUG
			qDebug() << "Airport not found: " << callsign;
#endif
		}
		
		return;
		
	}
}

