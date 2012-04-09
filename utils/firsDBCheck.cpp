/*
 * =====================================================================================
 *
 * firsDBCheck.cpp
 * Copyright (C) 2011 Michał Garapich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

#pragma pack(1)
struct Point {
	double x;
	double y;
};

struct FirHeader {
	char	icao[8];
	Point	externities[2];
	Point	textPosition;
};

struct Fir {
	FirHeader header;
	vector< Point > coords;
};
#pragma pack()

int
main(int argc, char** argv) {
	string fName = "WorldFirs.db";
	set< string > reqicaos;

	for (int i = 1; i < argc; ++i)
		reqicaos.insert(argv[i]);
	
	fstream db(fName.c_str(), ios::in | ios::binary);

	int size;
	db.read((char*)&size, 4);
	
	cout << "Firs in database: " << size << endl;
	
	db.seekg(4);
	
	Fir* firs = new Fir[size];
	for (int i = 0; i < size; ++i) {
		db.read((char*)&firs[i].header, sizeof(FirHeader));
		int counting;
		db.read((char*)&counting, sizeof(int));
		firs[i].coords.resize(counting);
		db.read((char*)&firs[i].coords[0], sizeof(Point) * counting);
	}
	
	db.close();
	
	for (int i = 0; i < size; ++i) {
		if (reqicaos.find(string(firs[i].header.icao)) != reqicaos.end()) {
			cout << "Fir ICAO:\t" << firs[i].header.icao << endl;
			for (Point& p: firs[i].coords)
				cout << p.x << " " << p.y << endl;
		}
	}

	delete [] firs;
	
	cout << "FIRs database ok." << endl;

	return 0;
}



