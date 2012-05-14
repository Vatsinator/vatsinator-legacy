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

#include <qgl.h>

#include "db/AirportsDatabase.h"

#include "ui/mapwidget/Clickable.h"
#include "ui/mapwidget/Controller.h"
#include "ui/mapwidget/Pilot.h"

class AirportObject : public Clickable {
	
public:
	
	AirportObject(const QString& = "");
	virtual ~AirportObject();
	
	void addStaff(const Controller*);
	
	void addInbound(const Pilot*);
	
	void addOutbound(const Pilot*);
	
	unsigned countDepartures() const;
	unsigned countArrivals() const;
	
	bool hasApproach() const;
	
	unsigned getFacilities() const;
	
	inline const QVector< const Controller* > &
	getStaff() const { return __staff; }
	
	inline const QVector< const Pilot* > &
	getInbounds() const { return __inbounds; }
	
	inline const QVector< const Pilot* > &
	getOutbounds() const { return __outbounds; }

	inline const AirportRecord *
	getData() const { return __data; }
	
	inline ObjectType
	objectType() const { return AIRPORT; }
	
	GLuint labelTip;
	
	
private:
	void __generateTip();
	
	const AirportRecord *	__data;
	
	QVector< const Controller* > __staff;
	
	QVector< const Pilot* > __inbounds;
	QVector< const Pilot* > __outbounds;
	
	
};

#endif // AIRPORTOBJECT_H
