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
		this,						SLOT(__updateData()));
}

void
FirDetailsWindow::show(const Fir* _f) {
	__currentICAO = _f->header.icao;
	
	__fillLabels(_f);
	__updateModels(_f);
	__setButtons();
	__adjustTables();
	
	QWidget::show();
}

void
FirDetailsWindow::__updateModels(const Fir* _f) {
	const Fir* f;
	if (!_f)
		f = FirsDatabase::GetSingleton().findFirByIcao(__currentICAO);
	else
		f = _f;
	
	Q_ASSERT(f);
	
	FlightsTable->setModel(_f->getFlightsModel());
	ATCTable->setModel(_f->getStaffModel());
	AirportsTable->setModel(_f->getAirportsModel());
}

void
FirDetailsWindow::__fillLabels(const Fir* _f) {
	if (_f->country != "USA")
		setWindowTitle(static_cast< QString >(_f->header.icao) + " - FIR details");
	else
		setWindowTitle(static_cast< QString >(_f->header.icao) + " - ARTCC details");
	
	ICAOLabel->setText(static_cast< QString >(_f->header.icao));
	NameLabel->setText(_f->name);
}

void
FirDetailsWindow::__adjustTables() {
	FlightsTable->hideColumn(FlightTableModel::Name);
	// 150, 160, 160, 150, 120
	FlightsTable->setColumnWidth(FlightTableModel::Callsign, 150);
	FlightsTable->setColumnWidth(FlightTableModel::From, 160);
	FlightsTable->setColumnWidth(FlightTableModel::To, 160);
	FlightsTable->setColumnWidth(FlightTableModel::Aircraft, 150);
	
	ATCTable->setColumnWidth(ControllerTableModel::Callsign, 150);
	ATCTable->setColumnWidth(ControllerTableModel::Name, 320);
	ATCTable->setColumnWidth(ControllerTableModel::Frequency, 150);
	
	AirportsTable->setColumnWidth(AirportTableModel::Label, 250);
	AirportsTable->setColumnWidth(AirportTableModel::Facilities, 170);
	AirportsTable->setColumnWidth(AirportTableModel::Outbounds, 100);
	AirportsTable->setColumnWidth(AirportTableModel::Inbounds, 100);
}

void
FirDetailsWindow::__setButtons() {
	const FlightTableModel* flightsModel = qobject_cast< const FlightTableModel* >(FlightsTable->model());
	Q_ASSERT(flightsModel);
	for (int i = 0; i < flightsModel->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(flightsModel->getFlights()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			FlightDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		FlightsTable->setIndexWidget(flightsModel->index(i, FlightTableModel::Button), pButton);
	}
	
	const ControllerTableModel* atcModel = qobject_cast< const ControllerTableModel* >(ATCTable->model());
	for (int i = 0; i < atcModel->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(atcModel->getStaff()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			ATCDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		ATCTable->setIndexWidget(atcModel->index(i, ControllerTableModel::Button), pButton);
	}
	
	const AirportTableModel* apModel = qobject_cast< const AirportTableModel* >(AirportsTable->model());
	for (int i = 0; i < apModel->rowCount(); ++i) {
		ShowAirportButton* pButton = new ShowAirportButton(apModel->getAirports()[i]);
		connect(pButton,				SIGNAL(clicked(const AirportObject*)),
			AirportDetailsWindow::GetSingletonPtr(),SLOT(show(const AirportObject*)));
		AirportsTable->setIndexWidget(apModel->index(i, AirportTableModel::Button), pButton);
	}
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
FirDetailsWindow::__updateData() {
	if (__currentICAO.isEmpty() || !isVisible())
		return;
	
	//__updateModels();
	__setButtons();
	//__adjustTables();
}




