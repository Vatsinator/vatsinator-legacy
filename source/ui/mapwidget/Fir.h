/*
    Fir.h
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

#include <GL/gl.h>

#include "ui/mapwidget/Clickable.h"
#include "ui/mapwidget/Controller.h"

#pragma pack(1)
struct Point {
	GLdouble x;
	GLdouble y;
};

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
	
	void generateTip();
	
	inline const QVector< const Controller* > &
	getStaff() const { return __staff; }
	
	inline const QVector< const Controller* > &
	getUirStaff() const { return __uirStaff; }
	
	inline void
	clear() { __staff.clear(); __uirStaff.clear(); }
	
	
	
	FirHeader		header;
	QString			name;	// FIR name
	QString			country; // country the FIR belongs to
	QVector< Point >	coords;
	
	GLuint	icaoTip;
	
private:
	
	QVector< const Controller* > __staff;
	QVector< const Controller* > __uirStaff;
	
	
};

#endif // FIR_H
