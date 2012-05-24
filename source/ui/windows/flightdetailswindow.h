/*
    flightdetailswindow.h
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


#ifndef FLIGHTDETAILSWINDOW_H
#define FLIGHTDETAILSWINDOW_H

#include "ui/ui_flightdetailswindow.h"
#include "singleton.h"

class Client;
class Pilot;

class FlightDetailsWindow :
		public QWidget,
		public Singleton< FlightDetailsWindow >,
		private Ui::FlightDetailsWindow {
	
	Q_OBJECT
	
public:
	FlightDetailsWindow(QWidget* = 0);
	
signals:
	void flightTrackingStateChanged(const Pilot*, int);
	
public slots:
	void showWindow(const Client*);
	void stateHandle(int);
	
private:
	void __setWindowPosition();
	
	const Pilot *	__current;
	
	
};

#endif // FLIGHTDETAILSWINDOW_H
