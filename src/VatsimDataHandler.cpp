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
	__pilots.clear();
	__atcs.clear();
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
			
			if (clientData[3] == "ATC") {
				Controller client;
				
				client.callsign = clientData[0];
				client.pid = clientData[1].toUInt();
				client.realName = clientData[2];
				
				client.frequency = clientData[4];
				client.onlineFrom = QDateTime::fromString(clientData[37], "yyyyMMddhhmmss");
				
				__atcs.push_back(client);
				
				QString icao = __obtainIcao(client.callsign);
				if (icao != "") {
					if (!__activeAirports.contains(icao))
						__activeAirports.insert(icao, AirportObject(icao));
					
					__activeAirports[icao].addStuff(client);
				}
				
			} else {
				Pilot client;
				
				client.callsign = clientData[0];
				client.pid = clientData[1].toUInt();
				client.realName = clientData[2];
				client.position.latitude = clientData[5].toDouble();
				client.position.longitude = clientData[6].toDouble();
				client.altitude = clientData[7].toInt();
				client.groundSpeed = clientData[8].toInt();
				client.aircraft = clientData[9];
				client.tas = clientData[10].toInt();
				client.route.origin = clientData[11];
				client.route.altitude = clientData[12];
				client.route.destination = clientData[13];
				client.server = clientData[14];
				client.flightRules = (clientData[21] == "I") ? IFR : VFR;
				client.remarks = clientData[29];
				client.route.route = clientData[30];
				client.onlineFrom = QDateTime::fromString(clientData[37], "yyyyMMddhhmmss");
				client.heading = clientData[38].toUInt();
				
				__pilots.push_back(client);
				
				if (!__activeAirports.contains(client.route.origin))
					__activeAirports.insert(client.route.origin, AirportObject(client.route.origin));
				
				if (!__activeAirports.contains(client.route.destination))
					__activeAirports.insert(client.route.destination, AirportObject(client.route.destination));		
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
	for (Pilot& p: __pilots)
		if (p.callsign == _callsign)
			return &p;
	return NULL;
}

void
VatsimDataHandler::__clearFlags(QMap< QString, bool >& _flags) {
	for (auto it = _flags.begin(); it != _flags.end(); ++it)
		it.value() = false;
}

QString
VatsimDataHandler::__obtainIcao(const QString& _atcCallsign) {
	QString icao = _atcCallsign.section('_', 0, 0);
	if (icao.length() == 4)
		return icao;
	else
		return "";
}
