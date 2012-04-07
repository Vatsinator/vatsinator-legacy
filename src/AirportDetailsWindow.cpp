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


#include "../include/AirportDetailsWindow.h"

#include "../include/AirportsDatabase.h"
#include "../include/AirportObject.h"
#include "../include/MetarsHandler.h"
#include "../include/OpenGLWidget.h"
#include "../include/ShowButton.h"

AirportDetailsWindow::AirportDetailsWindow(QWidget* _parent) :
		QWidget(_parent),
		__current(NULL),
		__openGLWidget(OpenGLWidget::GetSingletonPtr()) {
	setupUi(this);
	__setWindowPosition();
	
	QStringList labels = {
		"Callsign",
		"From",
		"Aircraft",
		"" };
	
	InboundTable->setHorizontalHeaderLabels(labels);
	InboundTable->setColumnWidth(0, 100);
	InboundTable->setColumnWidth(1, 280);
	InboundTable->setColumnWidth(2, 80);
	//InboundTable->setColumnWidth(4, 70);
	
	labels[1] = "To";
	OutboundTable->setHorizontalHeaderLabels(labels);
	OutboundTable->setColumnWidth(0, 100);
	OutboundTable->setColumnWidth(1, 280);
	OutboundTable->setColumnWidth(2, 80);
	//OutboundTable->setColumnWidth(4, 70);
	
	labels[1] = "Name";
	labels[2] = "Freq";
	labels[3] = "Rating";
	ATCTable->setHorizontalHeaderLabels(labels);
	ATCTable->setColumnWidth(0, 100);
	ATCTable->setColumnWidth(1, 280);
	ATCTable->setColumnWidth(2, 100);
	ATCTable->setColumnWidth(3, 80);
	
	connect(MetarsHandler::GetSingletonPtr(), SIGNAL(newMetarsAvailable(const QVector< Metar >&)),
			this, SLOT(updateMetar(const QVector< Metar >&)));
	
	__ratings[1] = "OBS";
	__ratings[2] = "S1";
	__ratings[3] = "S2";
	__ratings[4] = "S3";
	__ratings[5] = "C1";
	__ratings[7] = "C3";
	__ratings[8] = "I1";
	__ratings[10] = "I3";
	__ratings[11] = "SUP";
	__ratings[12] = "ADM";
	
}

void
AirportDetailsWindow::showWindow(const AirportObject* _ap) {
	AirportsDatabase& apdb = AirportsDatabase::GetSingleton();
	if (!__openGLWidget)
		__openGLWidget = OpenGLWidget::GetSingletonPtr();
	
	setWindowTitle((QString)_ap->getData()->icao + " - airport details");
	
	if (_ap->getData()->iata)
		CodesLabel->setText((QString)_ap->getData()->icao + "/" + _ap->getData()->iata);
	else
		CodesLabel->setText((QString)_ap->getData()->icao);
	
	NameLabel->setText((QString)_ap->getData()->name + ", " + _ap->getData()->city);
	
	int row = 0;
	InboundTable->clearContents();
	InboundTable->setRowCount(_ap->getInbounds().size());
	for (const Pilot* p: _ap->getInbounds()) {
		QTableWidgetItem *pCallsign = new QTableWidgetItem(p->callsign);
		pCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pFrom;
		Airport* origap = apdb.find(p->route.origin);
		if (origap)
			pFrom = new QTableWidgetItem(p->route.origin + " " +
					apdb.find(p->route.origin)->city);
		else
			pFrom = new QTableWidgetItem(p->route.origin);
		QTableWidgetItem *pAircraft = new QTableWidgetItem(p->aircraft);
		pAircraft->setTextAlignment(Qt::AlignCenter);
		
		ShowButton* showButton = new ShowButton(p);
		if (p->flightStatus == ARRIVED)
			showButton->setEnabled(false);
		else
			connect(showButton, SIGNAL(buttonClicked(const Pilot*)),
					this, SLOT(handleShowClicked(const Pilot*)));
		
		InboundTable->setItem(row, 0, pCallsign);
		InboundTable->setItem(row, 1, pFrom);
		InboundTable->setItem(row, 2, pAircraft);
		InboundTable->setCellWidget(row, 3, showButton);
		
		++row;
	}
	
	row = 0;
	OutboundTable->clearContents();
	OutboundTable->setRowCount(_ap->getOutbounds().size());
	for (const Pilot* p: _ap->getOutbounds()) {
		QTableWidgetItem *pCallsign = new QTableWidgetItem(p->callsign);
		pCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pTo;
		Airport* destap = apdb.find(p->route.destination);
		if (destap)
			pTo = new QTableWidgetItem(p->route.destination + " " +
					apdb.find(p->route.destination)->city);
		else
			pTo = new QTableWidgetItem(p->route.destination);
		QTableWidgetItem *pAircraft = new QTableWidgetItem(p->aircraft);
		pAircraft->setTextAlignment(Qt::AlignCenter);
		
		ShowButton* showButton = new ShowButton(p);
		if (p->flightStatus == DEPARTING)
			showButton->setEnabled(false);
		else
			connect(showButton, SIGNAL(buttonClicked(const Pilot*)),
					this, SLOT(handleShowClicked(const Pilot*)));
		
		OutboundTable->setItem(row, 0, pCallsign);
		OutboundTable->setItem(row, 1, pTo);
		OutboundTable->setItem(row, 2, pAircraft);
		OutboundTable->setCellWidget(row, 3, showButton);
		
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
		
		QTableWidgetItem* cRating = new QTableWidgetItem(__ratings[c->rating]);
		cRating->setTextAlignment(Qt::AlignCenter);
		
		ATCTable->setItem(row, 0, cCallsign);
		ATCTable->setItem(row, 1, cName);
		ATCTable->setItem(row, 2, cFreq);
		ATCTable->setItem(row, 3, cRating);
		
		++row;
	}
	
	MetarsHandler::GetSingleton().fetchMetar(_ap->getData()->icao);
	MetarLabel->setText("Fetching...");
	
	__current = _ap;
	
	show();
}

void
AirportDetailsWindow::updateMetar(const QVector< Metar >& _metars) {
	for (const Metar& m: _metars) {
		if (m.icao == __current->getData()->icao) {
			MetarLabel->setText(m.metar);
			return;
		}
	}
	
	MetarsHandler::GetSingleton().fetchMetar(__current->getData()->icao);
}

void
AirportDetailsWindow::handleShowClicked(const Pilot* _p) {
	hide();
	__openGLWidget->showPilot(_p);
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

