/*
    FirsDatabase.cpp
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
#include <sstream>
#include <fstream>
#include <string>

#include <QtGui>

#include "../include/FirsDatabase.h"

#include "../include/defines.h"

using namespace std;

void
FirsDatabase::init() {
	__firs.clear();
	
	fstream db(FIRS_DB, ios::in | ios::binary);

	int size;
	db.read((char*)&size, 4);
	
	db.seekg(4);
	
	__firs.resize(size);
	for (int i = 0; i < size; ++i) {
		db.read((char*)&__firs[i].header, sizeof(FirHeader));
		int counting;
		db.read((char*)&counting, sizeof(int));
		__firs[i].coords.resize(counting);
		db.read((char*)&__firs[i].coords[0], sizeof(Point) * counting);
	}
	
	db.close();
	
#ifndef NO_DEBUG
	qDebug() << "Firs read: " << size;
#endif
	
	clearAll();
}

Fir *
FirsDatabase::findFirByIcao(const QString& _icao) {
	for (Fir& f: __firs)
		if (f.header.icao == _icao)
			return &f;
	return NULL;
}

void
FirsDatabase::clearAll() {
	for (Fir& f: __firs)
		f.clear();
}
