/*
    AirportDetailsWindow.cpp
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

#include "ui/buttons/detailsbutton.h"
#include "ui/buttons/showpilotbutton.h"
#include "ui/mapwidget/airportobject.h"
#include "ui/mapwidget/mapwidget.h"

#include "vatsimdata/metarshandler.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "airportdetailswindow.h"
#include "defines.h"

// in-tabs columns sizes
const int COLUMN_WIDTHS [] = {
	150, 320, 150, 120
};

AirportDetailsWindow::AirportDetailsWindow(QWidget* _parent) :
		QWidget(_parent),
		__currentICAO("") {
	setupUi(this);
	__setWindowPosition();
	
	QStringList labels = {
		"Callsign",
		"From",
		"Aircraft",
		"" };
	
	InboundTable->setHorizontalHeaderLabels(labels);
	InboundTable->setColumnWidth(0, COLUMN_WIDTHS[0]);
	InboundTable->setColumnWidth(1, COLUMN_WIDTHS[1]);
	InboundTable->setColumnWidth(2, COLUMN_WIDTHS[2]);
	InboundTable->setColumnWidth(3, COLUMN_WIDTHS[3]);
	
	labels[1] = "To";
	OutboundTable->setHorizontalHeaderLabels(labels);
	OutboundTable->setColumnWidth(0, COLUMN_WIDTHS[0]);
	OutboundTable->setColumnWidth(1, COLUMN_WIDTHS[1]);
	OutboundTable->setColumnWidth(2, COLUMN_WIDTHS[2]);
	OutboundTable->setColumnWidth(3, COLUMN_WIDTHS[3]);
	
	labels[1] = "Name";
	labels[2] = "Freq";
	labels[3] = "";
	ATCTable->setHorizontalHeaderLabels(labels);
	ATCTable->setColumnWidth(0, COLUMN_WIDTHS[0]);
	ATCTable->setColumnWidth(1, COLUMN_WIDTHS[1]);
	ATCTable->setColumnWidth(2, COLUMN_WIDTHS[2]);
	ATCTable->setColumnWidth(3, COLUMN_WIDTHS[3]);
	
	connect(MetarsHandler::GetSingletonPtr(),	SIGNAL(newMetarsAvailable()),
		this,					SLOT(updateMetar()));
	connect(VatsinatorApplication::GetSingletonPtr(),	SIGNAL(dataUpdated()),
		this,						SLOT(__updateContents()));
	
}

void
AirportDetailsWindow::showWindow(const AirportObject* _ap) {
	__updateContents(_ap);
	
	if (_ap->getData())
		__currentICAO = _ap->getData()->icao;
	else
		__currentICAO = "";
	
	const Metar* m = MetarsHandler::GetSingleton().find(__currentICAO);
	if (m)
		MetarLabel->setText(m->metar);
	else {
		MetarLabel->setText("Fetching...");
		MetarsHandler::GetSingleton().fetchMetar(__currentICAO);
	}
	
	show();
}

void
AirportDetailsWindow::updateMetar() {
	if (__currentICAO.isEmpty())
		return;
	
	const Metar* m = MetarsHandler::GetSingleton().find(__currentICAO);
	if (m)
		MetarLabel->setText(m->metar);
}

void
AirportDetailsWindow::handleShowClicked(const Pilot* _p) {
	hide();
	emit showPilotRequest(_p);
}

void
AirportDetailsWindow::handleDetailsClicked(const Controller* _c) {
	hide();
	emit showATCDetailsRequest(_c);
}

void
AirportDetailsWindow::__updateContents(const AirportObject* _ap) {
	AirportsDatabase& apdb = AirportsDatabase::GetSingleton();
	
	setWindowTitle(static_cast< QString >(_ap->getData()->icao) + " - airport details");
	
	if (!static_cast< QString >(_ap->getData()->iata).isEmpty())
		CodesLabel->setText(static_cast< QString >(_ap->getData()->icao) + "/" +
			static_cast< QString >(_ap->getData()->iata));
	else
		CodesLabel->setText(static_cast< QString >(_ap->getData()->icao));
	
	NameLabel->setText(static_cast< QString >(_ap->getData()->name) + ", " +
			static_cast< QString >(_ap->getData()->city)
#ifndef NO_DEBUG
			// display FIR info only in debug builds
			+ " (" + 
			static_cast< QString >(_ap->getData()->fir) + " FIR)"
#endif
		);
	
	int row = 0;
	InboundTable->clearContents();
	InboundTable->setRowCount(_ap->getInbounds().size());
	for (const Pilot* p: _ap->getInbounds()) {
		QTableWidgetItem *pCallsign = new QTableWidgetItem(p->callsign);
		pCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pFrom;
		AirportRecord* origap = apdb.find(p->route.origin);
		if (origap)
			pFrom = new QTableWidgetItem(p->route.origin + " " +
					origap->city);
		else
			pFrom = new QTableWidgetItem(p->route.origin);
		QTableWidgetItem *pAircraft = new QTableWidgetItem(p->aircraft);
		pAircraft->setTextAlignment(Qt::AlignCenter);
		
		InboundTable->setItem(row, 0, pCallsign);
		InboundTable->setItem(row, 1, pFrom);
		InboundTable->setItem(row, 2, pAircraft);
		
		if (p->flightStatus == ARRIVED) {
			QTableWidgetItem* pArrived = new QTableWidgetItem("Arrived");
			pArrived->setTextAlignment(Qt::AlignCenter);
			InboundTable->setItem(row, 3, pArrived);
		} else {
			ShowPilotButton* showButton = new ShowPilotButton(p);
			connect(showButton,	SIGNAL(clicked(const Pilot*)),
				this,		SLOT(handleShowClicked(const Pilot*)));
			InboundTable->setCellWidget(row, 3, showButton);
		}
		
		++row;
	}
	
	row = 0;
	OutboundTable->clearContents();
	OutboundTable->setRowCount(_ap->getOutbounds().size());
	for (const Pilot* p: _ap->getOutbounds()) {
		QTableWidgetItem *pCallsign = new QTableWidgetItem(p->callsign);
		pCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pTo;
		AirportRecord* destap = apdb.find(p->route.destination);
		if (destap)
			pTo = new QTableWidgetItem(p->route.destination + " " +
					destap->city);
		else
			pTo = new QTableWidgetItem(p->route.destination);
		QTableWidgetItem *pAircraft = new QTableWidgetItem(p->aircraft);
		pAircraft->setTextAlignment(Qt::AlignCenter);
		
		OutboundTable->setItem(row, 0, pCallsign);
		OutboundTable->setItem(row, 1, pTo);
		OutboundTable->setItem(row, 2, pAircraft);
		
		if (p->flightStatus == DEPARTING) {
			QTableWidgetItem* pDeparting = new QTableWidgetItem("Departing");
			pDeparting->setTextAlignment(Qt::AlignCenter);
			OutboundTable->setItem(row, 3, pDeparting);
		} else {
			ShowPilotButton* showButton = new ShowPilotButton(p);
			connect(showButton,	SIGNAL(clicked(const Pilot*)),
				this,		SLOT(handleShowClicked(const Pilot*)));
			OutboundTable->setCellWidget(row, 3, showButton);
		}
		
		++row;
	}
	
	row = 0;
	ATCTable->clearContents();
	ATCTable->setRowCount(_ap->getStaff().size());
	for (const Controller* c: _ap->getStaff()) {
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
			
		++row;
	}
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

void
AirportDetailsWindow::__updateContents() {
	if (!isVisible())
		return;
	
	AirportObject* ap = VatsimDataHandler::GetSingleton().getActiveAirports()[__currentICAO];
	if (!ap)
		hide();
	
	__updateContents(ap);
}

