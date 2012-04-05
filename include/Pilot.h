/*
    Pilot.h
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


#ifndef PILOT_H
#define PILOT_H

#include <QDateTime>
#include <QString>

#include "Clickable.h"
#include "Client.h"

enum FlightRules {
	IFR, VFR
};

enum Status {
	DEPARTING,
	AIRBORNE,
	ARRIVED
};

struct Position {
	double	longitude;
	double	latitude;
};

struct Route {
	QString	origin;
	QString	destination;
	QString	route;
	QString	altitude;
};

class Pilot : public Client, public Clickable {
	
	/*
	 * This class contains info about one particular pilot - his
	 * flight plan, actual position, etc etc
	 */
	
public:
	
	ClientType type() const { return PILOT; }
	ObjectType objectType() const { return PLANE; }
	
	unsigned	pid;
	
	Position	position;
	int			altitude;
	int			groundSpeed;
	short		squawk;
	
	QString 	callsign;
	QString		realName;
	QString		aircraft;
	
	int			tas;
	
	Route		route;
	
	QString		server;
	
	FlightRules	flightRules;
	
	QString		remarks;
	
	QDateTime	onlineFrom;
	unsigned	heading;
	
	Status		flightStatus;
	
};

#endif // PILOT_H
