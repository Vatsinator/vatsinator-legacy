/*
    Controller.h
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


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>

#include "ui/mapwidget/client.h"

struct AirportRecord;

enum Facility {
	ATIS = 1,
	DEL = 2,
	GND = 4,
	TWR = 8,
	APP = 16,
	CTR = 32,
	FSS = 64,
	OBS = 128
};

class Controller : public Client {

public:
	Controller(const QStringList&);
	
	ClientType type() const { return ATC; }
	
	QString		frequency;
	int		rating;
	QString		icao;
	
	QString		atis;
	
	const AirportRecord*	airport;
	
	Facility	facility;
	
private:
	void __setMyIcaoAndFacility();
	
};

#endif // CONTROLLER_H
