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
#include <fstream>

#include <QtGui>

#include "vatsinatorapplication.h"

#include "firsdatabase.h"
#include "defines.h"

using namespace std;

FirsDatabase::FirsDatabase() {
#ifndef NO_DEBUG
	std::cout << "FIRS_DB: " << FIRS_DB << std::endl;
#endif
	__readDatabase();
}

void
FirsDatabase::init() {
	if (__toolTipsPrepared)
		return;
	
#ifndef NO_DEBUG
	qDebug() << "Preparing VBOs for FIRs...";
#endif
	
	for (Fir& f: __firs)
		f.init();
	
	__toolTipsPrepared = true;

#ifndef NO_DEBUG
	qDebug() << "FIRs' VBOs prepared.";
#endif
}

void
FirsDatabase::__readDatabase() {
	__toolTipsPrepared = false;
	__firs.clear();
	
	fstream db(FIRS_DB, ios::in | ios::binary);

	int size;
	db.read(reinterpret_cast< char* >(&size), 4);
	
#ifndef NO_DEBUG
	qDebug() << "Firs to be read: " << size;
#endif
	
	db.seekg(4);
	
	__firs.resize(size);
	for (int i = 0; i < size; ++i) {
		db.read(reinterpret_cast< char* >(&__firs[i].header), sizeof(FirHeader));
		int counting;
		db.read(reinterpret_cast< char* >(&counting), sizeof(int));
		__firs[i].borders.resize(counting);
		db.read(reinterpret_cast< char* >(&__firs[i].borders[0]), sizeof(Point) * counting);
		
		db.read(reinterpret_cast< char* >(&counting), sizeof(int));
		if (counting) {
			__firs[i].triangles.resize(counting * 3);
			db.read(reinterpret_cast< char* >(&__firs[i].triangles[0]), 2 * counting * 3);
		}
	}
	
	db.close();
	
	clearAll();
}

Fir *
FirsDatabase::findFirByIcao(const QString& _icao, bool _fss) {
	for (Fir& f: __firs)
		if (static_cast< QString >(f.header.icao) == _icao) {
			if (!f.header.oceanic && _fss)
				continue;
			if (f.header.oceanic && !_fss)
				continue;
			if (f.header.textPosition.x != 0 && f.header.textPosition.y != 0)
				return &f;
			else
				continue;
		}
	return NULL;
}

void
FirsDatabase::clearAll() {
	for (Fir& f: __firs)
		f.clear();
}
