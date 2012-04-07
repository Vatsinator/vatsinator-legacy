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
	 * This class contains data about all connected clients, needed URLs and
	 * active airports.
	 */
	
public:
	/**
	 * Default ctor.
	 */
	VatsimDataHandler();
	
	/**
	 * Destructor deletes all pointers.
	 */
	~VatsimDataHandler();
	
	/**
	 * This function parses the raw "status.txt" file. That file is fetched
	 * by VatsinatorApplication class and given exactly here.
	 */
	void parseStatusFile(const QString&);
	
	/**
	 * This function parses data file. Its URL can be obtained by getDataUrl()
	 * function and is choosen randomly from servers list.
	 * During parsing new Pilots, Controllers and AirportObjects instances are
	 * created.
	 */
	void parseDataFile(const QString&);
	
	/**
	 * Chooses randomly one of URLs and returns it.
	 */
	const QString& getDataUrl();
	
	/**
	 * Finds pilot by callsign and returns pointer. If not found, returns
	 * NULL.
	 */
	const Pilot * findPilot(const QString&);
	
	/**
	 * Returns an URL to where METARs can be fetched from.
	 */
	inline
	const QString & getMetarUrl() { return __metarURL; }
	
	/**
	 * The following functions return const references to vectors of clients.
	 */
	inline
	const QVector< Pilot* > & getPilots() { return __pilots; }
	inline
	const QVector< Controller* > & getATCs() { return __atcs; }
	inline
	const QMap< QString, AirportObject* > & getActiveAirports() { return __activeAirports; }
	
private:
	
	/**
	 * Parses one line from the data file.
	 */
	void __parseATC(const QStringList&);
	void __parsePilot(const QStringList&);
	
	/**
	 * Chooses if the pilot is departing, airborne or has just arrived.
	 */
	void __setStatus(Pilot*);
	
	/**
	 * Checks the ATC's position and airport.
	 */
	void __setIcaoAndFacility(Controller*);
	
	/**
	 * Clears the flags used during parsing the data file.
	 */
	void __clearFlags(QMap< QString, bool >&);
	
	/* These are vectors of connected clients */
	QVector< Pilot* >		__pilots;
	QVector< Controller* >	__atcs;
	
	/* This is vector of data servers, obtained from status file */
	QVector< QString >		__servers;
	
	/* This set contains list of active airports, used later by OpenGLWidget */
	QMap< QString, AirportObject* > __activeAirports;
	
	/* This is URL that we can obtain METAR from */
	QString		__metarURL;
	
	unsigned	__clientsConnected;
	
	AirportsDatabase & __airports;
	
	VatsinatorApplication &	__mother;
	
	/**
	 * Calculates the distance between two points.
	 */
	inline
	double __calcDistance(const double& _ax, const double& _ay,
								 const double& _bx, const double& _by) {
		return sqrt(
			pow(_ax - _bx, 2) +
			pow(_ay - _by, 2)
		);
	}
	
};

typedef QMap< QString, AirportObject* > AirportsMap;

#endif // VATSIMDATAHANDLER_H
