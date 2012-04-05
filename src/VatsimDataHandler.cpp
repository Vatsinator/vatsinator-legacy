/*
    VatsimDataHandler.cpp
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

#include <cstring>
#include <cstdlib>
#include <ctime>

#include <QStringList>
#include <QDebug>

#include "../include/VatsimDataHandler.h"

#include "../include/VatsinatorApplication.h"

#include "../include/defines.h"


VatsimDataHandler::VatsimDataHandler() :
		__airports(AirportsDatabase::GetSingleton()),
		__mother(VatsinatorApplication::GetSingleton()) {}

VatsimDataHandler::~VatsimDataHandler() {
	while (!__pilots.empty())
		delete __pilots.back(), __pilots.pop_back();
	while (!__atcs.empty())
		delete __atcs.back(), __atcs.pop_back();
	for (auto it = __activeAirports.begin(); it != __activeAirports.end(); ++it)
		delete it.value();
}

void
VatsimDataHandler::parseStatusFile(const QString& _statusFile) {
	QStringList tempList = _statusFile.split('\n', QString::SkipEmptyParts);
	for (QString& temp: tempList) {
		if (temp.startsWith(';'))
			continue;
		
		if (temp.startsWith("metar0=")) {
			__metarURL = temp.mid(7);
			__metarURL = __metarURL.simplified();
			continue;
		}
		
		if (temp.startsWith("url0=")) {
			QString url0 = temp.mid(5);
			url0 = url0.simplified();
			__servers.push_back(url0);

			continue;
		}
	}
}

void
VatsimDataHandler::parseDataFile(const QString& _data) {
	while (!__pilots.empty())
		delete __pilots.back(), __pilots.pop_back();
	__pilots.clear();
	while (!__atcs.empty())
		delete __atcs.back(), __atcs.pop_back();
	__atcs.clear();
	for (auto it = __activeAirports.begin(); it != __activeAirports.end(); ++it)
		delete it.value();
	__activeAirports.clear();
	
	qDebug() << "Data length: " << _data.length();
	QStringList tempList = _data.split('\n', QString::SkipEmptyParts);
	
	QMap< QString, bool > flags;
	flags["GENERAL"] = false;
	flags["CLIENTS"] = false;
	
	for (QString& temp: tempList) {
		if (temp.startsWith(';'))
			continue;
		
		if (temp.startsWith('!')) {
			__clearFlags(flags);
			if (temp.simplified() == "!GENERAL:")
				flags["GENERAL"] = true;
			if (temp.simplified() == "!CLIENTS:")
				flags["CLIENTS"] = true;
			
			continue;
		}
		
		if (flags["CLIENTS"]) {
			QStringList clientData = temp.split(':');
			
			/*
			 * Ok, this is out important parsing section. Let's set things up:
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
			
			if (clientData[3] == "ATC") {
				Controller* client = new Controller;
				
				client->callsign = clientData[0];
				client->pid = clientData[1].toUInt();
				client->realName = clientData[2];
				
				client->frequency = clientData[4];
				client->onlineFrom = QDateTime::fromString(clientData[37], "yyyyMMddhhmmss");
				
				__atcs.push_back(client);
				
				QString icao = __obtainIcao(client->callsign);
				if (icao != "") {
					if (!__activeAirports.contains(icao))
						__activeAirports.insert(icao, new AirportObject(icao));
					
					__activeAirports[icao]->addStuff(client);
				}
				
			} else {
				Pilot* client = new Pilot;
				
				client->callsign = clientData[0];
				client->pid = clientData[1].toUInt();
				client->realName = clientData[2];
				client->position.latitude = clientData[5].toDouble();
				client->position.longitude = clientData[6].toDouble();
				client->altitude = clientData[7].toInt();
				client->groundSpeed = clientData[8].toInt();
				client->aircraft = clientData[9];
				client->tas = clientData[10].toInt();
				client->route.origin = clientData[11];
				client->route.altitude = clientData[12];
				client->route.destination = clientData[13];
				client->server = clientData[14];
				client->squawk = clientData[17].toShort();
				client->flightRules = (clientData[21] == "I") ? IFR : VFR;
				client->remarks = clientData[29];
				client->route.route = clientData[30];
				client->onlineFrom = QDateTime::fromString(clientData[37], "yyyyMMddhhmmss");
				client->heading = clientData[38].toUInt();
				
				__pilots.push_back(client);
				
				if (!__activeAirports.contains(client->route.origin))
					__activeAirports.insert(client->route.origin, new AirportObject(client->route.origin));
				__activeAirports[client->route.origin]->addOutbound(client);
				
				if (!__activeAirports.contains(client->route.destination))
					__activeAirports.insert(client->route.destination, new AirportObject(client->route.destination));
				__activeAirports[client->route.destination]->addInbound(client);
				
				__setStatus(client);
			}
		}
		
	}
}

const QString &
VatsimDataHandler::getDataUrl() {
	srand(time(NULL));
	
	return __servers[rand() % __servers.size()];
}

const Pilot *
VatsimDataHandler::findPilot(const QString& _callsign) {
	for (const Pilot* p: __pilots)
		if (p->callsign == _callsign)
			return p;
	return NULL;
}

void
VatsimDataHandler::__clearFlags(QMap< QString, bool >& _flags) {
	for (auto it = _flags.begin(); it != _flags.end(); ++it)
		it.value() = false;
}

void
VatsimDataHandler::__setStatus(Pilot* _pilot) {
	if (!_pilot->route.origin.isEmpty()) { // we have flight plan, ok
		const Airport* ap_origin = __activeAirports[_pilot->route.origin]->getData();
		const Airport* ap_arrival = __activeAirports[_pilot->route.destination]->getData();
	
		if ((ap_origin == ap_arrival) && (ap_origin != NULL)) // traffic pattern?
			if (_pilot->groundSpeed < 25) {
				_pilot->flightStatus = DEPARTING;
				return;
			}
	
		if (ap_origin)
			// check if origin airport is in range
			if ((__calcDistance(ap_origin->longitude, ap_origin->latitude,
					_pilot->position.longitude, _pilot->position.latitude) < 0.1) && (_pilot->groundSpeed < 50)) {
				_pilot->flightStatus = DEPARTING;
				return;
			}
	
		if (ap_arrival)
			// or maybe arrival?
			if ((__calcDistance(ap_arrival->longitude, ap_arrival->latitude,
				_pilot->position.longitude, _pilot->position.latitude) < 0.1) && (_pilot->groundSpeed < 50)) {
				_pilot->flightStatus = ARRIVED;
				return;
			}
	} else { // no flight plan, we have to check where exactly the pilot is
		if (_pilot->groundSpeed > 30) {
			_pilot->flightStatus = AIRBORNE;
			return;
		}
		
		const Airport* closest = NULL;
		double distance = 0.0;
		for (const Airport& ap: __airports.getAirports()) { // yeah, this is messy
			double temp = __calcDistance(ap.longitude, ap.latitude,
										 _pilot->position.longitude, _pilot->position.latitude);
			if (((temp < distance) && closest) || !closest) {
				closest = &ap;
				distance = temp;
			}
		}
		
		if (closest) { // we found something really close
			_pilot->route.origin = closest->icao;
			if (!__activeAirports.contains(_pilot->route.origin))
				__activeAirports.insert(_pilot->route.origin, new AirportObject(_pilot->route.origin));
			__activeAirports[_pilot->route.origin]->addOutbound(_pilot);
			__setStatus(_pilot);
			return;
		}
	}
	
	// finally:
	_pilot->flightStatus = AIRBORNE;
}

QString
VatsimDataHandler::__obtainIcao(const QString& _atcCallsign) {
	QString icao = _atcCallsign.section('_', 0, 0);
	if (icao.length() == 4)
		return icao;
	else
		return "";
}
