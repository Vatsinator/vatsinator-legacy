/*
    flightlistwindow.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include <QtWidgets>

#include "ui/userinterface.h"
#include "ui/windows/vatsinatorwindow.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "flightlistwindow.h"

FlightListWindow::FlightListWindow(QWidget* parent) :
    BaseWindow(parent) {
  setupUi(this);
  
  connect(qApp,         SIGNAL(aboutToQuit()),
          this,         SLOT(hide()));
  connect(FlightsTable, SIGNAL(doubleClicked(const QModelIndex&)),
          this,         SLOT(__handleDoubleClicked(const QModelIndex&)));
}

void
FlightListWindow::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  __resizeColumns();
}

void
FlightListWindow::showEvent(QShowEvent* event) {
  if (auto m = FlightsTable->model())
    m->deleteLater();
  
  FlightsTable->setModel(vApp()->vatsimDataHandler()->flightTableModel());
  __resizeColumns();
  
  BaseWindow::showEvent(event);
}

void
FlightListWindow::__resizeColumns() {
  static Q_DECL_CONSTEXPR int CallsignWidth = 100;
  static Q_DECL_CONSTEXPR int AircraftWidth = 120;
  
  const int scrollbarWidth = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

  int spaceLeft = FlightsTable->width() - CallsignWidth - AircraftWidth - scrollbarWidth;
  spaceLeft /= 3;

  FlightsTable->setColumnWidth(FlightTableModel::Callsign, CallsignWidth);
  FlightsTable->setColumnWidth(FlightTableModel::Name, spaceLeft);
  FlightsTable->setColumnWidth(FlightTableModel::From, spaceLeft);
  FlightsTable->setColumnWidth(FlightTableModel::To, spaceLeft);
}

void
FlightListWindow::__handleDoubleClicked(const QModelIndex& index) {
  Q_ASSERT(qobject_cast<const FlightTableModel*>(index.model()));

  vApp()->userInterface()->showDetails(
    (qobject_cast<const FlightTableModel*>(index.model()))->flights()[index.row()]
  );
}
