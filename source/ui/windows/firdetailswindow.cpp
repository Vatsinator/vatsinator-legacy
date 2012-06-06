/*
    firdetailswindow.cpp
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

#include "ui/buttons/clientdetailsbutton.h"
#include "ui/buttons/showairportbutton.h"

#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"

#include "vatsimdata/airportobject.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/airporttablemodel.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "vatsinatorapplication.h"

#include "firdetailswindow.h"
#include "defines.h"

FirDetailsWindow::FirDetailsWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	__setWindowPosition();
	
	connect(VatsinatorApplication::GetSingletonPtr(),	SIGNAL(dataUpdated()),
		this,						SLOT(__updateContents()));
}

void
FirDetailsWindow::show(const Fir* _f) {
	__currentICAO = _f->header.icao;
	__updateContents(_f);
	QWidget::show();
}

void
FirDetailsWindow::__updateContents(const Fir* _f) {
	if (_f->country != "USA")
		setWindowTitle(static_cast< QString >(_f->header.icao) + " - FIR details");
	else
		setWindowTitle(static_cast< QString >(_f->header.icao) + " - ARTCC details");
	
	ICAOLabel->setText(static_cast< QString >(_f->header.icao));
	NameLabel->setText(_f->name);
	
	FlightsTable->setModel(_f->getFlightsModel());
	for (int i = 0; i < _f->getFlightsModel()->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(_f->getFlightsModel()->getFlights()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			FlightDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		FlightsTable->setIndexWidget(_f->getFlightsModel()->index(i, FlightTableModel::Button), pButton);
	}
	
	ATCTable->setModel(_f->getStaffModel());
	for (int i = 0; i < _f->getStaffModel()->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(_f->getStaffModel()->getStaff()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			ATCDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		ATCTable->setIndexWidget(_f->getStaffModel()->index(i, ControllerTableModel::Button), pButton);
	}
	
	AirportsTable->setModel(_f->getAirportsModel());
	for (int i = 0; i < _f->getAirportsModel()->rowCount(); ++i) {
		ShowAirportButton* pButton = new ShowAirportButton(_f->getAirportsModel()->getAirports()[i]);
		connect(pButton,				SIGNAL(clicked(const AirportObject*)),
			AirportDetailsWindow::GetSingletonPtr(),SLOT(show(const AirportObject*)));
		AirportsTable->setIndexWidget(_f->getAirportsModel()->index(i, AirportTableModel::Button), pButton);
	}
	
	__adjustTables();
}

void
FirDetailsWindow::__adjustTables() {
	FlightsTable->hideColumn(FlightTableModel::Name);
	// 150, 160, 160, 150, 120
	FlightsTable->setColumnWidth(FlightTableModel::Callsign, 150);
	FlightsTable->setColumnWidth(FlightTableModel::From, 160);
	FlightsTable->setColumnWidth(FlightTableModel::To, 160);
	FlightsTable->setColumnWidth(FlightTableModel::Aircraft, 150);
	FlightsTable->setColumnWidth(FlightTableModel::Button, 120);
	
	ATCTable->setColumnWidth(ControllerTableModel::Callsign, 150);
	ATCTable->setColumnWidth(ControllerTableModel::Name, 320);
	ATCTable->setColumnWidth(ControllerTableModel::Frequency, 150);
	ATCTable->setColumnWidth(ControllerTableModel::Button, 120);
	
	AirportsTable->setColumnWidth(AirportTableModel::Label, 250);
	AirportsTable->setColumnWidth(AirportTableModel::Facilities, 170);
	AirportsTable->setColumnWidth(AirportTableModel::Outbounds, 100);
	AirportsTable->setColumnWidth(AirportTableModel::Inbounds, 100);
	AirportsTable->setColumnWidth(AirportTableModel::Button, 120);
}

void
FirDetailsWindow::__setWindowPosition() {
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
FirDetailsWindow::__updateContents() {
	if (!isVisible())
		return;
	
	Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(__currentICAO);
	Q_ASSERT(fir);
	
	__updateContents(fir);
}




