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

#include "../include/ATCDetailsWindow.h"

#include "../include/AirportsDatabase.h"
#include "../include/Controller.h"

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
}

void
ATCDetailsWindow::showWindow(const Client* _client) {
	if (_client->type() != ATC)
		return;
	
	const Controller* atc = static_cast< const Controller* >(_client);
	
	setWindowTitle(QString(atc->callsign + " - ATC details"));
	
	CallsignLabel->setText(atc->callsign);
	NameLabel->setText(atc->realName);
	FrequencyLabel->setText(atc->frequency);
	RatingLabel->setText(__ratings[atc->rating]);
	if (atc->airport)
		AirportLabel->setText((QString)atc->airport->icao + " " + atc->airport->name + ", " + atc->airport->city);
	else
		AirportLabel->setText("N/A");
	ServerLabel->setText(atc->server);
	TimeOnlineLabel->setText(atc->onlineFrom.toString("dd MMM yyyy, hh:mm"));
	
	AtisMessageField->setPlainText(atc->atis);
	
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

