/*
    ATCDetailsWindow.cpp
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

#include "db/AirportsDatabase.h"

#include "ui/mapwidget/Controller.h"
#include "ui/windows/AirportDetailsWindow.h"

#include "ATCDetailsWindow.h"
#include "defines.h"

ATCDetailsWindow::ATCDetailsWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	__setWindowPosition();
	
	// http://vateud.org/index.php?option=com_content&view=article&id=28&Itemid=133
	__ratings[1] = "Observer";
	__ratings[2] = "Student (S1)";
	__ratings[3] = "Student 2 (S2)";
	__ratings[4] = "Senior Student (S3)";
	__ratings[5] = "Controller (C1)";
	__ratings[7] = "Senior Controller (C3)";
	__ratings[8] = "Instructor (I1)";
	__ratings[10] = "Senior Instructor (I3)";
	__ratings[11] = "Supervisor";
	__ratings[12] = "Administrator";
	
	connect(AirportDetailsWindow::GetSingletonPtr(),SIGNAL(showATCDetailsRequest(const Controller*)),
		this,					SLOT(showWindow(const Controller*)));
}

void
ATCDetailsWindow::showWindow(const Client* _client) {
	if (_client->type() != ATC) {
#ifndef NO_DEBUG
		qDebug() << "ATCDetailsWindow: passing incompatible argument! Client type must be ATC.";
#endif
		return;
	}
	
	__showMe(static_cast< const Controller* >(_client));
}

void
ATCDetailsWindow::showWindow(const Controller* _c) {
	__showMe(_c);
}

void
ATCDetailsWindow::__showMe(const Controller* _c) {
	setWindowTitle(QString(_c->callsign + " - ATC details"));
	
	CallsignLabel->setText(_c->callsign);
	NameLabel->setText(_c->realName);
	FrequencyLabel->setText(_c->frequency);
	RatingLabel->setText(__ratings[_c->rating]);
	if (_c->airport)
		AirportLabel->setText((QString)_c->airport->icao + " " + _c->airport->name + ", " + _c->airport->city);
	else
		AirportLabel->setText("N/A");
	ServerLabel->setText(_c->server);
	TimeOnlineLabel->setText(_c->onlineFrom.toString("dd MMM yyyy, hh:mm"));
	
	AtisMessageField->setPlainText(_c->atis);
	
	show();
}

void
ATCDetailsWindow::__setWindowPosition() {
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

