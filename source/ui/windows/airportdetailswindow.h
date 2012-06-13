/*
    airportdetailswindow.h
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


#ifndef AIRPORTDETAILSWINDOW_H
#define AIRPORTDETAILSWINDOW_H

#include "ui/ui_airportdetailswindow.h"
#include "singleton.h"

class AirportObject;
class Controller;
class Pilot;

class AirportDetailsWindow :
		public QWidget,
		public Singleton< AirportDetailsWindow >,
		private Ui::AirportDetailsWindow {
	
	Q_OBJECT
	
public:
	AirportDetailsWindow(QWidget* = 0);
	
public slots:
	void show(const AirportObject*);
 	void updateMetar();
	
private:
	void __updateModels(const AirportObject* = NULL);
	void __fillLabels(const AirportObject*);
	void __adjustTables();
	void __setButtons();
	void __setWindowPosition();
	
	QString __currentICAO;
	
private slots:
	void __updateData();
	
};

#endif // AIRPORTDETAILSWINDOW_H