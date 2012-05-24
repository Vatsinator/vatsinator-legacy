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

#include "ui/buttons/detailsbutton.h"
#include "ui/buttons/showairportbutton.h"
#include "ui/buttons/showpilotbutton.h"
#include "ui/mapwidget/airportobject.h"
#include "ui/mapwidget/mapwidget.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "firdetailswindow.h"
#include "defines.h"

// tabs columns widths
const int COLUMNS_WIDTHS [] = {
	150, 160, 160, 150, 120
};

FirDetailsWindow::FirDetailsWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	__setWindowPosition();
	
	QStringList labels = {
		"Callsign",
		"From",
		"To",
		"Aircraft",
		"" };
	
	FlightsTable->setHorizontalHeaderLabels(labels);
	for (int i = 0; i < FlightsTable->columnCount(); ++i)
		FlightsTable->setColumnWidth(i, COLUMNS_WIDTHS[i]);
	
	labels[1] = "Name";
	labels[2] = "Freq";
	labels[3] = "";
	ATCTable->setHorizontalHeaderLabels(labels);
	ATCTable->setColumnWidth(0, COLUMNS_WIDTHS[0]);
	ATCTable->setColumnWidth(1, COLUMNS_WIDTHS[1] + COLUMNS_WIDTHS[2]);
	ATCTable->setColumnWidth(2, COLUMNS_WIDTHS[3]);
	ATCTable->setColumnWidth(3, COLUMNS_WIDTHS[4]);
	
	labels[0] = "Airport";
	labels[1] = "ATC";
	labels[2] = "Outbound";
	labels[3] = "Inbound";
	labels[4] = "";
	AirportsTable->setHorizontalHeaderLabels(labels);
	AirportsTable->setColumnWidth(0, 250);
	AirportsTable->setColumnWidth(1, 170);
	AirportsTable->setColumnWidth(2, 100);
	AirportsTable->setColumnWidth(3, 100);
	AirportsTable->setColumnWidth(4, 120);
	
	connect(VatsinatorApplication::GetSingletonPtr(),	SIGNAL(dataUpdated()),
		this,						SLOT(__updateContents()));
}

void
FirDetailsWindow::showWindow(const Fir* _f) {
	__currentICAO = _f->header.icao;
	__updateContents(_f);
	show();
}

void
FirDetailsWindow::handleShowAirportClicked(const AirportObject* _ap) {
	hide();
	emit showAirportRequest(_ap);
}

void
FirDetailsWindow::handleShowPilotClicked(const Pilot* _p) {
	hide();
	emit showPilotRequest(_p);
}

void
FirDetailsWindow::handleDetailsClicked(const Controller* _c) {
	hide();
	emit showATCDetailsRequest(_c);
}

void
FirDetailsWindow::__updateContents(const Fir* _f) {
	AirportsDatabase& apdb = AirportsDatabase::GetSingleton();
	
	if (_f->country != "USA")
		setWindowTitle(static_cast< QString >(_f->header.icao) + " - FIR details");
	else
		setWindowTitle(static_cast< QString >(_f->header.icao) + " - ARTCC details");
	
	ICAOLabel->setText(static_cast< QString >(_f->header.icao));
	NameLabel->setText(_f->name);
	
	int row = 0;
	FlightsTable->clearContents();
	FlightsTable->setRowCount(_f->getFlights().size());
	for (const Pilot* p: _f->getFlights()) {
		QTableWidgetItem* pCallsign = new QTableWidgetItem(p->callsign);
		pCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pFrom;
		AirportRecord* origAp = apdb.find(p->route.origin);
		if (origAp)
			pFrom = new QTableWidgetItem(p->route.origin + " " +
					origAp->city);
		else
			pFrom = new QTableWidgetItem(p->route.origin);
		
		QTableWidgetItem* pTo;
		AirportRecord* destAp = apdb.find(p->route.destination);
		if (destAp)
			pTo = new QTableWidgetItem(p->route.destination + " " +
					destAp->city);
		else
			pTo = new QTableWidgetItem(p->route.destination);
		
		
		QTableWidgetItem* pAircraft = new QTableWidgetItem(p->aircraft);
		pAircraft->setTextAlignment(Qt::AlignCenter);
		
		FlightsTable->setItem(row, 0, pCallsign);
		FlightsTable->setItem(row, 1, pFrom);
		FlightsTable->setItem(row, 2, pTo);
		FlightsTable->setItem(row, 3, pAircraft);
		
		ShowPilotButton* showButton = new ShowPilotButton(p);
		connect(showButton,	SIGNAL(clicked(const Pilot*)),
			this,		SLOT(handleShowPilotClicked(const Pilot*)));
		FlightsTable->setCellWidget(row, 4, showButton);
		
		row += 1;
	}
	
	row = 0;
	ATCTable->clearContents();
	ATCTable->setRowCount(_f->getStaff().size() + _f->getUirStaff().size());
	for (const Controller* c: _f->getStaff()) {
		QTableWidgetItem* cCallsign = new QTableWidgetItem(c->callsign);
		cCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* cName = new QTableWidgetItem(c->realName);
		cName->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* cFreq = new QTableWidgetItem(c->frequency);
		cFreq->setTextAlignment(Qt::AlignCenter);
		
		DetailsButton* detailsButton = new DetailsButton(c);
		connect(detailsButton,	SIGNAL(clicked(const Controller*)),
			this,		SLOT(handleDetailsClicked(const Controller*)));
		
		ATCTable->setItem(row, 0, cCallsign);
		ATCTable->setItem(row, 1, cName);
		ATCTable->setItem(row, 2, cFreq);
		
		ATCTable->setCellWidget(row, 3, detailsButton);
		
		row += 1;
	}
	
	for (const Controller* c: _f->getUirStaff()) {
		QTableWidgetItem* cCallsign = new QTableWidgetItem(c->callsign);
		cCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* cName = new QTableWidgetItem(c->realName);
		cName->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* cFreq = new QTableWidgetItem(c->frequency);
		cFreq->setTextAlignment(Qt::AlignCenter);
		
		DetailsButton* detailsButton = new DetailsButton(c);
		connect(detailsButton,	SIGNAL(clicked(const Controller*)),
			this,		SLOT(handleDetailsClicked(const Controller*)));
		
		ATCTable->setItem(row, 0, cCallsign);
		ATCTable->setItem(row, 1, cName);
		ATCTable->setItem(row, 2, cFreq);
		
		ATCTable->setCellWidget(row, 3, detailsButton);
		
		row += 1;
	}
	
	row = 0;
	AirportsTable->clearContents();
	AirportsTable->setRowCount(_f->getAirports().size());
	for (const AirportObject* ap: _f->getAirports()) {
		QTableWidgetItem* apIcao = new QTableWidgetItem(" " %
				static_cast< QString >(ap->getData()->icao) %
				" " %
				static_cast< QString >(ap->getData()->city)
			);
		//apIcao->setTextAlignment(Qt::AlignCenter);
		
		QString atcText;
		unsigned apFacilities = ap->getFacilities();
		if (!apFacilities)
			atcText = "-";
		else {
			if (apFacilities & APP)
				atcText += "APP ";
			if (apFacilities & TWR)
				atcText += "TWR ";
			if (apFacilities & GND)
				atcText += "GND ";
			if (apFacilities & DEL)
				atcText += "DEL ";
			if (apFacilities & ATIS)
				atcText += "ATIS ";
			atcText = atcText.simplified();
		}
		
		QTableWidgetItem* apATC = new QTableWidgetItem(atcText);	
		apATC->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* apDepartures = new QTableWidgetItem(QString::number(ap->getOutbounds().size()));
		apDepartures->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* apArrivals = new QTableWidgetItem(QString::number(ap->getInbounds().size()));
		apArrivals->setTextAlignment(Qt::AlignCenter);
		
		ShowAirportButton* apShow = new ShowAirportButton(ap);
		connect(apShow,		SIGNAL(clicked(const AirportObject*)),
			this,		SLOT(handleShowAirportClicked(const AirportObject*)));
		
		AirportsTable->setItem(row, 0, apIcao);
		AirportsTable->setItem(row, 1, apATC);
		AirportsTable->setItem(row, 2, apDepartures);
		AirportsTable->setItem(row, 3, apArrivals);
		
		AirportsTable->setCellWidget(row, 4, apShow);
		
		row += 1;
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
FirDetailsWindow::__updateContents() {
	if (!isVisible())
		return;
	
	Fir* fir = FirsDatabase::GetSingleton().findFirByIcao(__currentICAO);
	if (!fir) // that should never happen
		hide();
	
	__updateContents(fir);
}




