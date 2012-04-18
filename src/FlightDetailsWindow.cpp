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

#include "../include/FlightDetailsWindow.h"

#include "../include/AirportsDatabase.h"
#include "../include/Pilot.h"
#include "../include/MapWidget.h"
#include "../include/UserInterface.h"
#include "../include/defines.h"

FlightDetailsWindow::FlightDetailsWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	
	connect(TrackFlightBox, SIGNAL(stateChanged(int)), this, SLOT(stateHandle(int)));
	
	__setWindowPosition();
}

void
FlightDetailsWindow::showWindow(const Client* _client) {
	if (_client->type() != PILOT)
		return;
	
	const Pilot* pilot = static_cast< const Pilot* >(_client);
	
	setWindowTitle(QString(pilot->callsign + " - flight details"));
	
	CallsignLabel->setText(pilot->callsign);
	RouteLabel->setText(pilot->route.origin + " -> " + pilot->route.destination);
	
	PilotLabel->setText(pilot->realName + " (" + QString::number(pilot->pid) + ")");
	AltitudeLabel->setText(QString::number(pilot->altitude) + " feet");
	GroundSpeedLabel->setText(QString::number(pilot->groundSpeed) + " kts");
	HeadingLabel->setText(QString::number(pilot->heading));
	
	if (pilot->flightStatus == AIRBORNE)
		CurrentStatusLabel->setText("airborne");
	else if (pilot->flightStatus == DEPARTING)
		CurrentStatusLabel->setText("departing");
	else
		CurrentStatusLabel->setText("arrived");
	
	ServerLabel->setText(pilot->server);
	TimeOnlineLabel->setText(pilot->onlineFrom.toString("dd MMM yyyy, hh:mm"));
	SquawkLabel->setText(QString::number(pilot->squawk));
	
	FlightRulesLabel->setText((pilot->flightRules == IFR) ? "IFR" : "VFR");
	
	AirportRecord* ap = AirportsDatabase::GetSingleton().find(pilot->route.origin);
	QString text = pilot->route.origin;
	if (ap)
		text.append(QString(" ") + QString::fromAscii(ap->name) + " - " + QString::fromUtf8(ap->city));
	OriginLabel->setText(text);
	
	ap = AirportsDatabase::GetSingleton().find(pilot->route.destination);
	text = pilot->route.destination;
	if (ap)
		text.append(QString(" ") + ap->name + " - " + ap->city);

	ArrivalLabel->setText(text);
	AircraftLabel->setText(pilot->aircraft);
	TrueAirSpeedLabel->setText(QString::number(pilot->tas) + " kts");
	CruiseAltitude->setText(pilot->route.altitude);
	
	RouteField->setPlainText(pilot->route.route);
	RemarksField->setPlainText(pilot->remarks);
	
	if (UserInterface::GetSingleton().getGLContext()->getTrackedPilot() == pilot)
		TrackFlightBox->setCheckState(Qt::Checked);
	else
		TrackFlightBox->setCheckState(Qt::Unchecked);
	
	__current = pilot;
	
	show();
}

void
FlightDetailsWindow::stateHandle(int _state) {
	switch(_state) {
		case Qt::Checked:
			UserInterface::GetSingleton().getGLContext()->getTrackedPilot() = __current;
			break;
		case Qt::Unchecked:
			UserInterface::GetSingleton().getGLContext()->getTrackedPilot() = NULL;
	}
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


