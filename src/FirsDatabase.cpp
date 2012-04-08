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
	
	fstream db(FIRS_DB, ios::in);
	
	unsigned coordsCount = 0;
	bool readingFir = false;
	string buffer, foo;
	qDebug() << "Reading...";
	while (!db.eof()) {
		getline(db, buffer);
		
		istringstream line(buffer);
		if (!readingFir) {
			Fir temp;
			string icao;
			line >> icao >> foo >> foo >> coordsCount >> temp.externites[0].y >>
					temp.externites[0].x >> temp.externites[1].y >>
					temp.externites[1].x >> temp.text.y >>
					temp.text.x;
			temp.icao = icao.c_str();
			temp.staffed = false;
			readingFir = true;
			__firs.push_back(temp);
		} else {
			Point temp;
			line >> temp.y >> temp.x;
			__firs.last().coords.push_back(temp);
			--coordsCount;
			if (!coordsCount)
				readingFir = false;
			//qDebug() << "Read: " << coordsCount;
		}
	}
	
	db.close();
}

Fir *
FirsDatabase::findFirByIcao(const QString& _icao) {
	for (Fir& f: __firs)
		if (f.icao == _icao)
			return &f;
	return NULL;
}
