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
#include "ui/mapwidget/client.h"

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
	Pilot(const QStringList&);
	virtual ~Pilot();
	
	ClientType type() const { return PILOT; }
	ObjectType objectType() const { return PLANE; }
	
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
		qreal	longitude;
		qreal	latitude;
	} position;
	
	struct {
		QString origin;
		QString destination;
		QString route;
		QString altitude;
	} route;
	
	GLuint	callsignTip;
	GLuint	modelTexture;
	
private:
	void __setMyStatus();
	void __generateTip();
	
};

#endif // PILOT_H
