/*
    AirportsDatabase.cpp
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

#include <fstream>

#include <QtGui>

#include "VatsinatorApplication.h"

#include "AirportsDatabase.h"

#include "defines.h"

using namespace std;

AirportsDatabase::AirportsDatabase() {}

void
AirportsDatabase::init() {
	
	static_assert(sizeof(AirportRecord) == 428, "Sizeof Airport class is not 420 bytes! The WorldAirports database will be incompatible!");
	
	__airports.clear();
	
	fstream db(AIRPORTS_DB, ios::in | ios::binary);
	
	int size;
	db.read((char*)&size, 4);
	
#ifndef NO_DEBUG
	qDebug() << "Airports to be read: " << size;
#endif
	
	db.seekg(4);
	
	__airports.resize(size);
	db.read((char*)&__airports[0], sizeof(AirportRecord) * size);
	
	db.close();

}

AirportRecord *
AirportsDatabase::find(const QString& _key) {
	for (AirportRecord& a: __airports)
		if (static_cast< QString >(a.icao) == _key)
			return &a;
	
	return NULL;
}

