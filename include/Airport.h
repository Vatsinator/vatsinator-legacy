/*
    Airport.h
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


#ifndef AIRPORT_H
#define AIRPORT_H

#pragma pack(1)
struct Airport {
	
	/*
	 * This struct is compatible with utils/airportDBCreator.py script that
	 * generates the database.
	 */
	
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

#endif // AIRPORT_H
