/*
    pilot.h
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


#ifndef PILOT_H
#define PILOT_H

#include <QStringList>
#include <QString>

#include <GL/gl.h>

#include "ui/mapwidget/clickable.h"
#include "vatsimdata/client.h"

enum FlightRules {
	IFR, VFR
};

enum Status {
	DEPARTING,
	AIRBORNE,
	ARRIVED
};

class Pilot : public Client, public Clickable {
	
	/*
	 * This class contains info about one particular pilot - his
	 * flight plan, actual position, plane, etc etc
	 */
	
public:
	/**
	 * @param list Data.
	 * @param prefiled Indicates whether the flight is only prefiled.
	 */
	Pilot(const QStringList&, bool = false);
	virtual ~Pilot();
	
	void drawLineFrom() const;
	void drawLineTo() const;
	
	inline void
	drawLines() const { drawLineFrom(); drawLineTo(); }
	
	ClientType type() const { return PILOT; }
	ObjectType objectType() const { return PLANE; }
	
	inline GLuint
	getCallsignTip() const { return __callsignTip ? __callsignTip : __generateTip(); }
	
	int		altitude;
	int		groundSpeed;
	QString		squawk;
	
	QString		aircraft;
	
	int		tas;
	
	FlightRules	flightRules;
	
	QString		remarks;
	
	unsigned	heading;
	
	Status		flightStatus;
	
	struct {
		float	latitude;
		float	longitude;
	} position;
	
	struct {
		QString origin;
		QString destination;
		QString route;
		QString altitude;
	} route;
	
	GLuint	modelTexture;
	
	bool prefiledOnly;
	
private:
	/**
	 * Checks whether pilot is departing, airborn or has just arrived.
	 */
	void __setMyStatus();
	
	/**
	 * Generates in/out lines.
	 */
	void __generateLines();
	
	/**
	 * Generates the callsign label.
	 */
	GLuint __generateTip() const;
	
	GLfloat *__lineFrom;
	GLfloat *__lineTo;
	
	mutable GLuint __callsignTip;
	
};

#endif // PILOT_H
