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

#include "../include/AirportsDatabase.h"

#include "../include/VatsinatorApplication.h"

#include "../include/defines.h"

using namespace std;

AirportsDatabase::AirportsDatabase() :
		__mother(VatsinatorApplication::GetSingleton()) {}

void
AirportsDatabase::init() {
	
	static_assert(sizeof(Airport) == 420, "Sizeof Airport class is not 420 bytes! The WorldAirports database will be incompatible!");
	
	__airports.clear();
	
	fstream db(AIRPORTS_DB, ios::in | ios::binary);
	
	int size;
	db.read((char*)&size, 4);
	
	db.seekg(4);
	
	__airports.resize(size);
	db.read((char*)&__airports[0], sizeof(Airport) * size);
	
	db.close();
	
	qDebug() << "Airports read: " << size;

}

Airport *
AirportsDatabase::find(const QString& _key) {
	for (Airport& a: __airports)
		if (a.icao == _key)
			return &a;
	
	return NULL;
}

