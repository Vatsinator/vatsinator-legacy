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

#include <QDateTime>
#include <QString>

#include "Client.h"

struct AirportRecord;

enum Facility {
	ATIS,
	DEL,
	GND,
	TWR,
	APP,
	CTR,
	FSS,
	OBS
};

class Controller : public Client {

public:
	
	ClientType type() const { return ATC; }
	
	unsigned	pid;
	
	QString 	callsign;
	QString		realName;
	
	QString		frequency;
	QString		server;
	int		rating;
	
	QString		atis;
	
	QDateTime	onlineFrom;
	
	const AirportRecord*	airport;
	
	Facility	facility;
	
};

#endif // CONTROLLER_H
