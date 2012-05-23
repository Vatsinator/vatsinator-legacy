/*
    FlightsListWindow.cpp
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

#include "ui/buttons/showpilotbutton.h"
#include "ui/mapwidget/pilot.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "flightslistwindow.h"
#include "defines.h"

const int ROW_HEIGHT = 20;

FlightsListWindow::FlightsListWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	__setWindowPosition();
	
	QStringList labels = {
		"Callsign",
		"Name",
		"From",
		"To",
		"Aircraft",
		""
	};
	
	FlightsTable->setHorizontalHeaderLabels(labels);
	
	__setColumnsWidths();
	
	connect(VatsinatorApplication::GetSingletonPtr(),	SIGNAL(dataUpdated()),
		this,						SLOT(__updateContents()));
	
	connect(FlightsTable,	SIGNAL(cellDoubleClicked(int, int)),
		this,		SLOT(__handleCellClicked(int, int)));
}

FlightsListWindow::~FlightsListWindow() {
	FlightsTable->clearContents();
}

void
FlightsListWindow::handleShowClicked(const Pilot* _p) {
	hide();
	emit showPilotRequested(_p);
}

void
FlightsListWindow::resizeEvent(QResizeEvent* _event) {
	QWidget::resizeEvent(_event);
	__setColumnsWidths();
}

void
FlightsListWindow::__setColumnsWidths() {
	const int CALLSIGN_SIZE = 100;
	const int ACFT_SIZE = 120;
	const int SHOW_SIZE = 100;
	const int SCROLLBAR_SIZE = 30;
	
	int spaceLeft = FlightsTable->width() - CALLSIGN_SIZE - ACFT_SIZE - SHOW_SIZE - SCROLLBAR_SIZE;
	spaceLeft /= 3;
	
	FlightsTable->setColumnWidth(0, CALLSIGN_SIZE);
	FlightsTable->setColumnWidth(1, spaceLeft);
	FlightsTable->setColumnWidth(2, spaceLeft);
	FlightsTable->setColumnWidth(3, spaceLeft);
	FlightsTable->setColumnWidth(4, ACFT_SIZE);
	FlightsTable->setColumnWidth(5, SHOW_SIZE);
}

void
FlightsListWindow::__setWindowPosition() {
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
FlightsListWindow::__updateContents() {
	AirportsDatabase& apdb = AirportsDatabase::GetSingleton();
	
	FlightsTable->clearContents();
	FlightsTable->setRowCount(VatsimDataHandler::GetSingleton().getPilots().size());
	
	int row = 0;
	for (const Pilot* p: VatsimDataHandler::GetSingleton().getPilots()) {
		FlightsTable->setRowHeight(row, ROW_HEIGHT);
		
		QTableWidgetItem* pCallsign = new QTableWidgetItem(p->callsign);
		pCallsign->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pName = new QTableWidgetItem(p->realName);
		pName->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pFrom;
		AirportRecord* apFrom = apdb.find(p->route.origin);
		if (apFrom)
			pFrom = new QTableWidgetItem(p->route.origin % " " %
						     static_cast< QString >(apFrom->city)
				);
		else
			pFrom = new QTableWidgetItem(p->route.origin);
		pFrom->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pTo;
		AirportRecord* apTo = apdb.find(p->route.destination);
		if (apTo)
			pTo= new QTableWidgetItem(p->route.destination % " " %
						  static_cast< QString >(apTo->city)
				);
		else
			pTo = new QTableWidgetItem(p->route.origin);
		pTo->setTextAlignment(Qt::AlignCenter);
		
		QTableWidgetItem* pAcft = new QTableWidgetItem(p->aircraft);
		pAcft->setTextAlignment(Qt::AlignCenter);
		
		ShowPilotButton* pShow = new ShowPilotButton(p);
		connect(pShow,	SIGNAL(clicked(const Pilot*)),
			this,	SLOT(handleShowClicked(const Pilot*)));
		
		FlightsTable->setItem(row, 0, pCallsign);
		FlightsTable->setItem(row, 1, pName);
		FlightsTable->setItem(row, 2, pFrom);
		FlightsTable->setItem(row, 3, pTo);
		FlightsTable->setItem(row, 4, pAcft);
		FlightsTable->setCellWidget(row, 5, pShow);
		
		row += 1;
	}
}

void
FlightsListWindow::__handleCellClicked(int row, int) {
	emit showPilotRequested(dynamic_cast< const ShowPilotButton* >(FlightsTable->cellWidget(row, 5))->getPilot());
}
