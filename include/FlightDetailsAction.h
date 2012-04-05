/*
    FlightDetailsAction.h
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


#ifndef FLIGHTDETAILSACTION_H
#define FLIGHTDETAILSACTION_H

#include <QAction>

#include "Client.h"

class FlightDetailsAction :
		public QAction {
	
	Q_OBJECT
	
public:
	FlightDetailsAction(const Client*, QObject*);
	FlightDetailsAction(const QString&, const Client*, QObject*);
	
signals:
	void clicked(const Client*);
	
public slots:
	void handleTriggered();

private:
	const Client* __current;
	
	
};

#endif // FLIGHTDETAILSACTION_H
