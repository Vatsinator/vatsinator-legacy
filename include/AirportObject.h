/*
    AirportObject.h
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


#ifndef AIRPORTOBJECT_H
#define AIRPORTOBJECT_H

#include <QVector>

#include "AirportsDatabase.h"
#include "Clickable.h"
#include "Controller.h"
#include "Pilot.h"

class AirportObject : public Clickable {
	
public:
	
	AirportObject(const QString& = "");
	
	void addStuff(const Controller*);
	
	void addInbound(const Pilot*);
	
	void addOutbound(const Pilot*);
	
	unsigned countDepartures() const;
	unsigned countArrivals() const;
	
	const QVector< const Controller* > getStuff() const { return __staff; }
	
	const QVector< const Pilot* > getInbounds() const { return __inbounds; }
	const QVector< const Pilot* > getOutbounds() const { return __outbounds; }

	const Airport * getData() const { return __data; }
	
	ObjectType objectType() const { return AIRPORT; }
	
	
private:
	
	const Airport *	__data;
	
	QVector< const Controller* > __staff;
	
	QVector< const Pilot* > __inbounds;
	QVector< const Pilot* > __outbounds;
	
	
};

#endif // AIRPORTOBJECT_H
