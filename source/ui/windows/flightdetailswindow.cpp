/*
    FlightDetailsWindow.cpp
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

#include <QtGui>

#include "db/airportsdatabase.h"

#include "modules/flighttracker.h"

#include "ui/mapwidget/pilot.h"
#include "ui/mapwidget/mapwidget.h"
#include "ui/userinterface.h"

#include "flightdetailswindow.h"
#include "defines.h"

FlightDetailsWindow::FlightDetailsWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	
	connect(TrackFlightBox, SIGNAL(stateChanged(int)), this, SLOT(stateHandle(int)));
	
	__setWindowPosition();
}

void
FlightDetailsWindow::showWindow(const Client* _client) {
	if (!dynamic_cast< const Pilot* >(_client))
		return;
	
	__current = static_cast< const Pilot* >(_client);
	
	setWindowTitle(QString(__current->callsign + " - flight details"));
	
	CallsignLabel->setText(__current->callsign);
	RouteLabel->setText(__current->route.origin + " -> " + __current->route.destination);
	
	PilotLabel->setText(__current->realName + " (" + QString::number(__current->pid) + ")");
	AltitudeLabel->setText(QString::number(__current->altitude) + " feet");
	GroundSpeedLabel->setText(QString::number(__current->groundSpeed) + " kts");
	HeadingLabel->setText(QString::number(__current->heading));
	
	if (__current->flightStatus == AIRBORNE)
		CurrentStatusLabel->setText("airborne");
	else if (__current->flightStatus == DEPARTING)
		CurrentStatusLabel->setText("departing");
	else
		CurrentStatusLabel->setText("arrived");
	
	ServerLabel->setText(__current->server);
	TimeOnlineLabel->setText(__current->onlineFrom.toString("dd MMM yyyy, hh:mm"));
	SquawkLabel->setText(__current->squawk);
	
	FlightRulesLabel->setText((__current->flightRules == IFR) ? "IFR" : "VFR");
	
	AirportRecord* ap = AirportsDatabase::GetSingleton().find(__current->route.origin);
	QString text = __current->route.origin;
	if (ap)
		text.append(QString(" ") + QString::fromAscii(ap->name) + " - " + QString::fromUtf8(ap->city));
	OriginLabel->setText(text);
	
	ap = AirportsDatabase::GetSingleton().find(__current->route.destination);
	text = __current->route.destination;
	if (ap)
		text.append(QString(" ") + ap->name + " - " + ap->city);

	ArrivalLabel->setText(text);
	AircraftLabel->setText(__current->aircraft);
	TrueAirSpeedLabel->setText(QString::number(__current->tas) + " kts");
	CruiseAltitude->setText(__current->route.altitude);
	
	RouteField->setPlainText(__current->route.route);
	RemarksField->setPlainText(__current->remarks);
	
	if (FlightTracker::GetSingleton().getTracked() == __current)
		TrackFlightBox->setCheckState(Qt::Checked);
	else
		TrackFlightBox->setCheckState(Qt::Unchecked);
	
	show();
}

void
FlightDetailsWindow::stateHandle(int _state) {
	emit flightTrackingStateChanged(__current, _state);
}


void
FlightDetailsWindow::__setWindowPosition() {
	QDesktopWidget* desktop = QApplication::desktop();
	
	int screenWidth, width;
	int screenHeight, height;
	
	int x, y;
	
	QSize windowSize;
	
	screenWidth = desktop -> width();
	screenHeight = desktop -> height();
	
	windowSize = size();
	width = windowSize.width();
	height = windowSize.height();
	
	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;
	
	move(x, y);
}


