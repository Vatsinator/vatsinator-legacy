/*
    AirportDetailsWindow.h
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

#include "ui_AirportDetailsWindow.h"
#include "Metar.h"
#include "Singleton.h"

class AirportObject;
class OpenGLWidget;
class Pilot;

class AirportDetailsWindow :
		public QWidget,
		public Singleton< AirportDetailsWindow >,
		private Ui::AirportDetailsWindow {
	
	Q_OBJECT
	
public:
	AirportDetailsWindow(QWidget* = 0);
	
public slots:
	void showWindow(const AirportObject*);
	void updateMetar(const QVector< Metar >&);
	void handleShowClicked(const Pilot*);
	
private:
	void __setWindowPosition();
	
	const AirportObject* __current;
	
	QMap< int, QString > __ratings;
	
	OpenGLWidget*	__openGLWidget;
	
	
};

#endif // AIRPORTDETAILSWINDOW_H
