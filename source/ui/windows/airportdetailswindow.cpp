/*
    airportdetailswindow.cpp
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

#include "ui/buttons/clientdetailsbutton.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"

#include "vatsimdata/airportobject.h"
#include "vatsimdata/client.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/metarlistmodel.h"

#include "airportdetailswindow.h"
#include "defines.h"

AirportDetailsWindow::AirportDetailsWindow(QWidget* _parent) :
		QWidget(_parent),
		__currentICAO("") {
	setupUi(this);
	__setWindowPosition();
	
	connect(MetarListModel::GetSingletonPtr(),	SIGNAL(newMetarsAvailable()),
		this,					SLOT(updateMetar()));
}

void
AirportDetailsWindow::show(const AirportObject* _ap) {
	Q_ASSERT(_ap->getData());
	
	__currentICAO = _ap->getData()->icao;
	__fillLabels(_ap);
	
	const Metar* m = MetarListModel::GetSingleton().find(__currentICAO);
	if (m)
		MetarLabel->setText(m->getMetar());
	else {
		MetarLabel->setText("Fetching...");
		MetarListModel::GetSingleton().fetchMetar(__currentICAO);
	}
	
	InboundTable->setModel(_ap->getInboundsModel());
	for (int i = 0; i < _ap->getInboundsModel()->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(_ap->getInboundsModel()->getFlights()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			FlightDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		InboundTable->setIndexWidget(_ap->getInboundsModel()->index(i, FlightTableModel::Button), pButton);
	}
	
	OutboundTable->setModel(_ap->getOutboundsModel());
	for (int i = 0; i < _ap->getOutboundsModel()->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(_ap->getOutboundsModel()->getFlights()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			FlightDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		OutboundTable->setIndexWidget(_ap->getOutboundsModel()->index(i, FlightTableModel::Button), pButton);
	}
	
	ATCTable->setModel(_ap->getStaffModel());
	for (int i = 0; i < _ap->getStaffModel()->rowCount(); ++i) {
		ClientDetailsButton* pButton = new ClientDetailsButton(_ap->getStaffModel()->getStaff()[i]);
		connect(pButton,				SIGNAL(clicked(const Client*)),
			ATCDetailsWindow::GetSingletonPtr(),	SLOT(show(const Client*)));
		ATCTable->setIndexWidget(_ap->getStaffModel()->index(i, ControllerTableModel::Button), pButton);
	}
	
	__adjustTable();
	
	QWidget::show();
}

void
AirportDetailsWindow::updateMetar() {
	if (__currentICAO.isEmpty())
		return;
	
	const Metar* m = MetarListModel::GetSingleton().find(__currentICAO);
	if (m)
		MetarLabel->setText(m->getMetar());
}

void
AirportDetailsWindow::__fillLabels(const AirportObject* _ap) {
	setWindowTitle(static_cast< QString >(_ap->getData()->icao) + " - airports details");
	
	if (!static_cast< QString >(_ap->getData()->iata).isEmpty())
		CodesLabel->setText(
				static_cast< QString >(_ap->getData()->icao) %
				"/" %
				static_cast< QString >(_ap->getData()->iata)
			);
	else
		CodesLabel->setText(static_cast< QString >(_ap->getData()->icao));
	
	NameLabel->setText(
			static_cast< QString >(_ap->getData()->name) %
			", " %
			static_cast< QString >(_ap->getData()->city)
			
#ifndef NO_DEBUG
			//display FIR info only in debug mode
			% " (" %
			static_cast< QString >(_ap->getData()->fir) % " FIR)"
#endif
		);
}

void
AirportDetailsWindow::__adjustTable() {
	// make the table nice
	InboundTable->hideColumn(FlightTableModel::Name);
	InboundTable->hideColumn(FlightTableModel::To);
	
	InboundTable->setColumnWidth(FlightTableModel::Callsign, 150);
	InboundTable->setColumnWidth(FlightTableModel::From, 320);
	InboundTable->setColumnWidth(FlightTableModel::Aircraft, 150);
	InboundTable->setColumnWidth(FlightTableModel::Button, 120);
	
	OutboundTable->hideColumn(FlightTableModel::Name);
	OutboundTable->hideColumn(FlightTableModel::From);
	
	OutboundTable->setColumnWidth(FlightTableModel::Callsign, 150);
	OutboundTable->setColumnWidth(FlightTableModel::To, 320);
	OutboundTable->setColumnWidth(FlightTableModel::Aircraft, 150);
	OutboundTable->setColumnWidth(FlightTableModel::Button, 120);
	
	ATCTable->setColumnWidth(ControllerTableModel::Callsign, 150);
	ATCTable->setColumnWidth(ControllerTableModel::Name, 320);
	ATCTable->setColumnWidth(ControllerTableModel::Frequency, 150);
	ATCTable->setColumnWidth(ControllerTableModel::Button, 120);
}

void
AirportDetailsWindow::__setWindowPosition() {
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

