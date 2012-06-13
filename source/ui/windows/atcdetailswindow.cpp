/*
    atcdetailswindow.cpp
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
#include "db/firsdatabase.h"

#include "ui/mapwidget/mapwidget.h"

#include "vatsimdata/controller.h"

#include "atcdetailswindow.h"
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
	
	connect(ShowButton,	SIGNAL(clicked()), this, SLOT(__handleShowClicked()));
}

void
ATCDetailsWindow::show(const Client* _client) {
	Q_ASSERT(dynamic_cast< const Controller* >(_client));
	__current = dynamic_cast< const Controller* >(_client);
	
	setWindowTitle(QString(__current->callsign + " - ATC details"));
	
	CallsignLabel->setText(__current->callsign);
	FacilityLabel->setText(__produceFacility(__current));
	NameLabel->setText(__current->realName);
	FrequencyLabel->setText(__current->frequency);
	RatingLabel->setText(__ratings[__current->rating]);
	if (__current->airport)
		AirportLabel->setText(static_cast< QString >(__current->airport->icao) %
				" " %
				static_cast< QString >(__current->airport->name) %
				", " %
				static_cast< QString >(__current->airport->city));
		else
			AirportLabel->setText("N/A");
		ServerLabel->setText(__current->server);
	TimeOnlineLabel->setText(__current->onlineFrom.toString("dd MMM yyyy, hh:mm"));
	
	AtisMessageField->setPlainText(__current->atis);
	
	QWidget::show();
}

QString
ATCDetailsWindow::__produceFacility(const Controller* _c) {
	if (_c->facility == CTR) {
		const Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(_c->icao, false);
		if (!fir)
			fir = FirsDatabase::GetSingleton().findFirByIcao(_c->icao, true);
		if (fir)
			return fir->getName();
	}
	
	if (_c->facility == FSS) {
		const Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(_c->icao, true);
		if (!fir)
			fir = FirsDatabase::GetSingleton().findFirByIcao(_c->icao, false);
		if (fir)
			return fir->getName();
	}
	
	QString airport, facility;
	
	if (!_c->airport) {
		airport = "Unknown";
	} else {
		if (static_cast< QString >(_c->airport->name) == static_cast< QString >(_c->airport->city))
			airport = QString::fromUtf8(_c->airport->name);
		else
			airport =
				QString::fromUtf8(_c->airport->city) %
				"/" %
				QString::fromUtf8(_c->airport->name);
	}
	
	switch (_c->facility) {
		case ATIS:
			facility = "ATIS";
			break;
		case DEL:
			facility = "Delivery";
			break;
		case GND:
			facility = "Ground";
			break;
		case TWR:
			facility = "Tower";
			break;
		case APP:
			facility = "Approach";
			break;
		case DEP:
			facility = "Departure";
			break;
		default:
			break;
	}
	
	return airport % " " % facility;
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

void
ATCDetailsWindow::__handleShowClicked() {
	if (__current->airport) {
		MapWidget::GetSingleton().showPoint(__current->airport->longitude, __current->airport->latitude);
		return;
	}
	
	if (__current->facility == CTR) {
		const Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(__current->icao, false);
		if (!fir)
			fir = FirsDatabase::GetSingleton().findFirByIcao(__current->icao, true);
		if (fir) {
			MapWidget::GetSingleton().showPoint(fir->getTextPosition().x, fir->getTextPosition().y);
			return;
		}
	}
	
	if (__current->facility == FSS) {
		const Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(__current->icao, true);
		if (!fir)
			fir = FirsDatabase::GetSingleton().findFirByIcao(__current->icao, false);
		if (fir) {
			MapWidget::GetSingleton().showPoint(fir->getTextPosition().x, fir->getTextPosition().y);
			return;
		}
	}
}

