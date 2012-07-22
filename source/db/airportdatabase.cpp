/*
    airportdatabase.cpp
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

#include <iostream>
#include <fstream>

#include <QtGui>

#include "vatsinatorapplication.h"

#include "airportdatabase.h"
#include "defines.h"

AirportDatabase::AirportDatabase() {
  VatsinatorApplication::log("Airport database location: " AIRPORTS_DB);
}

void
AirportDatabase::init() {
  
  static_assert(
    sizeof(AirportRecord) == 436,
    "Sizeof Airport class is not 436 bytes! The WorldAirports database will be incompatible!"
  );
  
  __airports.clear();
  
  std::fstream db(AIRPORTS_DB, std::ios::in | std::ios::binary);

  int size;
  db.read((char*)&size, 4);

  VatsinatorApplication::log("Airports to be read: %i.", size);

  db.seekg(4);

  __airports.resize(size);
  db.read((char*)&__airports[0], sizeof(AirportRecord) * size);

  db.close();

}

const AirportRecord*
AirportDatabase::find(const QString& _key) {
  for (const AirportRecord & a: __airports)
    if (static_cast< QString >(a.icao) == _key)
      return &a;
  
  return NULL;
}

