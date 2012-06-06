/*
    fir.h
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


#ifndef FIR_H
#define FIR_H

#include <QVector>

#include <qgl.h>

#include "db/point.h"

#include "vatsimdata/airportobject.h"
#include "ui/mapwidget/clickable.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"

class AirportTableModel;
class ControllerTableModel;
class FlightTableModel;
class VertexBufferObject;

#pragma pack(1)
struct FirHeader {
	char	icao[4];
	int	oceanic; // 0 or 1
	Point	externities[2];
	Point	textPosition;
};
#pragma pack()

class Fir : public Clickable {
	
public:
	Fir();
	virtual ~Fir();
	
	ObjectType objectType() const { return FIR; }
	
	void addStaff(const Controller*);
	void addUirStaff(const Controller*);
	
	void addFlight(const Pilot*);
	
	void addAirport(const AirportObject*);
	
	void correctName();
	
	void init();
	void clear();
	bool isStaffed() const;
	
	void drawBorders() const;
	void drawTriangles() const;
	
	inline ControllerTableModel *
	getStaffModel() const { return __staff; }
	
	inline FlightTableModel *
	getFlightsModel() const { return __flights; }
	
	inline AirportTableModel *
	getAirportsModel() const { return __airports; }
	
	inline bool
	hasUirStaff() const { return __hasUirStaff; }
	
	
	
	FirHeader			header;
	QString				name;	// FIR name
	QString				country; // country the FIR belongs to
	QVector< Point >		borders;
	QVector< unsigned short >	triangles;
	
	GLuint	icaoTip;
	
private:
	
	void __generateTip();
	void __prepareVBO();
	
	ControllerTableModel *	__staff;
	FlightTableModel *	__flights;
	AirportTableModel *	__airports;
	
	bool __hasUirStaff;
	
	VertexBufferObject * __bordersVBO;
	VertexBufferObject * __trianglesVBO;
	
	unsigned	__bordersSize;
	unsigned	__trianglesSize;
};

#endif // FIR_H
