/*
    AirportsDatabase.h
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


#ifndef AIRPORTSDATABASE_H
#define AIRPORTSDATABASE_H

#include <QVector>

#include "Singleton.h"

#pragma pack(1)
struct AirportRecord {
	
	/*
	 * This struct is compatible with utils/airportDBCreator.py script that
	 * generates the database.
	 */
	
	char	name[128];
	char	city[128];
	char	country[128];
	char	iata[8];
	char	icao[8];
	double	latitude;
	double	longitude;
	int		altitude;
	
};
#pragma pack()

class VatsinatorApplication;

class AirportsDatabase :
		public Singleton< AirportsDatabase > {
	
	/*
	 * This class contains all needed info about airports.
	 */
	
public:
	
	/**
	 * Default ctor.
	 */
	AirportsDatabase();
	
	/**
	 * Reads the entire Airports database.
	 */
	void init();
	
	/**
	 * Gives access to the vector of airports.
	 */
	QVector< AirportRecord > & getAirports() { return __airports; }
	
	AirportRecord * find(const QString&);
	
private:
	
	QVector< AirportRecord > __airports;
	
	VatsinatorApplication &	__mother;
	
};

#endif // AIRPORTSDATABASE_H
