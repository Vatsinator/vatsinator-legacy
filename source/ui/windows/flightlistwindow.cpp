/*
    flightlistwindow.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include "ui/userinterface.h"

#include "ui/windows/flightdetailswindow.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsimdata/client/pilot.h"

#include "vatsimdata/models/flighttablemodel.h"

#include "flightlistwindow.h"
#include "defines.h"

FlightListWindow::FlightListWindow(QWidget* _parent) :
    BaseWindow(_parent) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));
  
  UserInterface::setWindowPosition(this);
  FlightsTable->setModel(VatsimDataHandler::getSingleton().flightsModel());
  FlightsTable->hideColumn(FlightTableModel::Button);
  __setColumnsWidths();

  connect(FlightsTable, SIGNAL(doubleClicked(const QModelIndex&)),
          this,         SLOT(__handleDoubleClicked(const QModelIndex&)));
}

void
FlightListWindow::resizeEvent(QResizeEvent* _event) {
  QWidget::resizeEvent(_event);
  __setColumnsWidths();
}

void
FlightListWindow::__setColumnsWidths() {
  static const int CALLSIGN_SIZE = 100;
  static const int ACFT_SIZE = 120;
  static const int SCROLLBAR_SIZE = 30;

  int spaceLeft = FlightsTable->width() - CALLSIGN_SIZE - ACFT_SIZE - SCROLLBAR_SIZE;
  spaceLeft /= 3;

  FlightsTable->setColumnWidth(FlightTableModel::Callsign, CALLSIGN_SIZE);
  FlightsTable->setColumnWidth(FlightTableModel::Name, spaceLeft);
  FlightsTable->setColumnWidth(FlightTableModel::From, spaceLeft);
  FlightsTable->setColumnWidth(FlightTableModel::To, spaceLeft);
}

void
FlightListWindow::__handleDoubleClicked(const QModelIndex& _index) {
  Q_ASSERT(qobject_cast<const FlightTableModel*>(_index.model()));

  FlightDetailsWindow::getSingleton().show(
    (qobject_cast< const FlightTableModel* >(_index.model()))->flights()[_index.row()]
  );
}
