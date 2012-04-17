/*
    AirportObject.cpp
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


#include "../include/AirportObject.h"
#include "../include/defines.h"

AirportObject::AirportObject(const QString& _icao) :
		__data(AirportsDatabase::GetSingleton().find(_icao)) {}

void
AirportObject::addStaff(const Controller* _c) {
	__staff.push_back(_c);
}

void
AirportObject::addInbound(const Pilot* _p) {
	__inbounds.push_back(_p);
}

void
AirportObject::addOutbound(const Pilot* _p) {
	__outbounds.push_back(_p);
}

unsigned
AirportObject::countDepartures() const {
	unsigned i = 0;
	for (const Pilot* p: __outbounds)
		if (p->flightStatus == DEPARTING)
			++i;
	
	return i;
}

unsigned
AirportObject::countArrivals() const {
	unsigned i = 0;
	for (const Pilot* p: __inbounds)
		if (p->flightStatus == ARRIVED)
			++i;
	
	return i;
}

bool
AirportObject::hasApproach() const {
	for (const Controller* c: __staff)
		if (c->facility == APP)
			return true;
	
	return false;
}

