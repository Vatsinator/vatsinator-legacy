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

class AirportObject : public Clickable {
	
public:
	
	AirportObject(const QString& = "");
	
	void addStuff(Controller);
	
	const QVector< Controller > getStuff() const { return __staff; }
	
	const Airport * getData() const { return __data; }
	
	ObjectType objectType() const { return AIRPORT; }
	
	
private:
	
	const Airport *	__data;
	
	QVector< Controller > __staff;
	
	
};

#endif // AIRPORTOBJECT_H
