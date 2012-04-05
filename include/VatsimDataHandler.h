/*
    VatsimDataHandler.h
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


#ifndef VATSIMDATAHANDLER_H
#define VATSIMDATAHANDLER_H

#include <QString>
#include <QVector>
#include <QMap>

#include "AirportsDatabase.h"
#include "AirportObject.h"
#include "Metar.h"
#include "Controller.h"
#include "Pilot.h"
#include "Singleton.h"

class VatsinatorApplication;

class VatsimDataHandler :
		public Singleton< VatsimDataHandler > {
	
	/*
	 * This class contains data about all connected clients.
	 */
	
public:
	VatsimDataHandler();
	
	void parseStatusFile(const QString&);
	
	void parseDataFile(const QString&);
	
	const QString& getDataUrl();
	
	const QString & getMetarUrl() { return __metarURL; }
	
	const QVector< Pilot > & getPilots() { return __pilots; }
	const QVector< Controller > & getATCs() { return __atcs; }
	
	const QMap< QString, AirportObject > & getActiveAirports() { return __activeAirports; }
	
	const Pilot * findPilot(const QString&);
	
private:
	
	void __clearFlags(QMap< QString, bool >&);
	
	QString __obtainIcao(const QString&);
	
	/* These are vectors of connected clients */
	QVector< Pilot >		__pilots;
	QVector< Controller >	__atcs;
	
	/* This is vector of data servers, obtained from status file */
	QVector< QString > __servers;
	
	/* This set contains list of active airports, used later by AirportDatabase */
	QMap< QString, AirportObject > __activeAirports;
	
	/* This is URL that we can obtain METAR from */
	QString	__metarURL;
	
	unsigned __clientsConnected;
	
	AirportsDatabase & __airports;
	
	VatsinatorApplication &	__mother;
	
};

typedef QMap< QString, AirportObject > AirportsMap;

#endif // VATSIMDATAHANDLER_H
