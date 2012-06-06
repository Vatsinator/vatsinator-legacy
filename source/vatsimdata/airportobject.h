/*
    airportobject.h
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

#include "ui/mapwidget/clickable.h"

struct AirportRecord;
class Controller;
class ControllerTableModel;
class FlightTableModel;
class Pilot;

class AirportObject : public Clickable {
	
public:
	
	AirportObject(const QString& = "");
	virtual ~AirportObject();
	
	void addStaff(const Controller*);
	
	void addInbound(const Pilot*);
	
	void addOutbound(const Pilot*);
	
	unsigned countDepartures() const;
	unsigned countOutbounds() const;
	unsigned countArrivals() const;
	unsigned countInbounds() const;
	
	bool hasApproach() const;
	
	unsigned getFacilities() const;
	
	inline ControllerTableModel *
	getStaffModel() const { return __staff; }
	
	inline FlightTableModel *
	getInboundsModel() const { return __inbounds; }
	
	inline FlightTableModel *
	getOutboundsModel() const { return __outbounds; }

	inline const AirportRecord *
	getData() const { return __data; }
	
	inline ObjectType
	objectType() const { return AIRPORT; }
	
	GLuint labelTip;
	
	
private:
	void __generateTip();
	
	const AirportRecord *	__data;
	
	ControllerTableModel * __staff;
	
	FlightTableModel * __inbounds;
	FlightTableModel * __outbounds;
	
	
};

#endif // AIRPORTOBJECT_H
