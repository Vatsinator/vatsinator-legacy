/*
 * =====================================================================================
 *
 * dbCheck.cpp
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
#include <set>

using namespace std;

#pragma pack(1)
struct AirportRecord {
	char	name[128];
	char	city[128];
	char	country[128];
	char	iata[8];
	char	icao[8];
	double	latitude;
	double	longitude;
	int		altitude;
};
#pragma pack()

int
main(int argc, char** argv) {
	string fName = "WorldAirports.db";
	set< string > reqicaos;

	for (int i = 1; i < argc; ++i)
		reqicaos.insert(argv[i]);
	
	fstream db(fName.c_str(), ios::in | ios::binary);

	int size;
	db.read((char*)&size, 4);

	cout << "Airports in database: " << size << endl;

	db.seekg(4);

	AirportRecord *aps = new AirportRecord[size];
	db.read((char*)aps, sizeof(AirportRecord)*size);

	db.close();

	for (int i = 0; i < size; ++i) {
//		cout << aps[i].icao << endl;
		if (reqicaos.find(string(aps[i].icao)) != reqicaos.end())
			cout << endl << "Name:\t\t" << aps[i].name << endl <<
				"City:\t\t" << aps[i].city << endl <<
				"Country:\t" << aps[i].country << endl <<
				"IATA:\t\t" << aps[i].iata << endl <<
				"ICAO:\t\t" << aps[i].icao << endl <<
				"Latitude:\t" << aps[i].latitude << endl <<
				"Longitude:\t" << aps[i].longitude << endl <<
				"Altitude:\t" << aps[i].altitude << endl;
	}

	delete [] aps;
	
	cout << "Airports database ok." << endl;

	return 0;
}


