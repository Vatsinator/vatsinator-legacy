/*
    firdetailswindow.cpp
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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "ui/userinterface.h"
#include "ui/buttons/clientdetailsbutton.h"
#include "ui/buttons/airportdetailsbutton.h"
#include "ui/models/airporttablemodel.h"
#include "ui/models/atcbookingtablemodel.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/models/notamlistmodel.h"
#include "ui/models/roles.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "vatsimdata/abstractbookingprovider.h"
#include "vatsimdata/abstractnotamprovider.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/pilot.h"
#include "vatsinatorapplication.h"

#include "firdetailswindow.h"

FirDetailsWindow::FirDetailsWindow(const Fir* fir, QWidget* parent) :
    BaseWindow(parent),
    __fir(fir) {
  setupUi(this);
  AtcTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  AirportsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  BookedATCTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  FlightsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  
  connect(qApp, &QCoreApplication::aboutToQuit, this, &FirDetailsWindow::close);
  connect(vApp()->vatsimDataHandler()->notamProvider(), &AbstractNotamProvider::notamReady,
          this, &FirDetailsWindow::__notamUpdate);
  connect(AirportsTable, &QTableView::doubleClicked, this, &FirDetailsWindow::__showAirportDetails);
  connect(AtcTable, &QTableView::doubleClicked, this, &FirDetailsWindow::__showClientDetails);
  connect(FlightsTable, &QTableView::doubleClicked, this, &FirDetailsWindow::__showClientDetails);
  connect(NotamTableView, &DelayedModelTableView::doubleClicked, this, &FirDetailsWindow::__goToNotam);
}

void
FirDetailsWindow::showEvent(QShowEvent* event) {
  __updateLabels();

  FlightsTable->setModel(__fir->flights());
  /* TODO Show UIR controllers here, too */
  AtcTable->setModel(__fir->staff());
  AirportsTable->setModel(__fir->airports());
  BookedATCTable->setModel(vApp()->vatsimDataHandler()->bookingProvider()->bookings(__fir->icao()));
  
  FlightsTable->hideColumn(FlightTableModel::Name);
  
  NotamTableView->setModel(nullptr);
  vApp()->vatsimDataHandler()->notamProvider()->fetchNotam(__fir->icao());
  NotamProviderInfoLabel->setText(vApp()->vatsimDataHandler()->notamProvider()->providerInfo());
  
  BaseWindow::showEvent(event);
}

void
FirDetailsWindow::__updateLabels() {
  if (__fir->country() != "USA")
    setWindowTitle(tr("%1 - FIR details").arg(__fir->icao()));
  else
    setWindowTitle(tr("%1 - ARTCC details").arg(__fir->icao()));

  if (!__fir->isOceanic())
    ICAOLabel->setText(__fir->icao());
  else
    ICAOLabel->setText(__fir->icao() + " Oceanic");

  NameLabel->setText(__fir->name());
}

void
FirDetailsWindow::__notamUpdate(NotamListModel* model) {
  if (model->icao() == __fir->icao()) {
    NotamTableView->setModel(model);
  }
}

void
FirDetailsWindow::__showAirportDetails(QModelIndex index) {
  Q_ASSERT(index.data(InstancePointerRole).isValid());
  Airport* const airport = reinterpret_cast<Airport* const>(index.data(InstancePointerRole).value<void*>());
  vApp()->userInterface()->showDetails(airport);
}

void
FirDetailsWindow::__showClientDetails(QModelIndex index) {
  Q_ASSERT(index.data(InstancePointerRole).isValid());
  Client* const client = reinterpret_cast<Client* const>(index.data(InstancePointerRole).value<void*>());
  vApp()->userInterface()->showDetails(client);
}

void
FirDetailsWindow::__goToNotam(QModelIndex index) {
  QString url = index.data(UrlRole).toString();
  if (!url.isEmpty())
    QDesktopServices::openUrl(url);
}
