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
#include "modules/vatbookhandler.h"
#include "modules/models/bookedatctablemodel.h"
#include "network/abstractnotamprovider.h"
#include "ui/userinterface.h"
#include "ui/buttons/clientdetailsbutton.h"
#include "ui/buttons/airportdetailsbutton.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/airporttablemodel.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/notamlistmodel.h"
#include "vatsinatorapplication.h"

#include "firdetailswindow.h"

FirDetailsWindow::FirDetailsWindow(const Fir* _fir, QWidget* _parent) :
    BaseWindow(_parent),
    __fir(_fir) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));
  connect(vApp()->vatsimDataHandler()->notamProvider(),
                                                SIGNAL(notamReady(NotamListModel*)),
          this,                                 SLOT(__notamUpdate(NotamListModel*)));
  connect(NotamTableView,                       SIGNAL(doubleClicked(QModelIndex)),
          this,                                 SLOT(__goToNotam(QModelIndex)));
}

void
FirDetailsWindow::show() {
  __fillLabels();
  __updateModels();
  __adjustTables();

  QWidget::show();
  
  NotamTableView->setModel(nullptr);
  vApp()->vatsimDataHandler()->notamProvider()->fetchNotam(__fir->icao());
  NotamProviderInfoLabel->setText(vApp()->vatsimDataHandler()->notamProvider()->providerInfo());
}

void
FirDetailsWindow::__updateModels() {
  Q_ASSERT(__fir);
  
  FlightsTable->setModel(__fir->flights());
  /* TODO Show UIR controllers here, too */
  ATCTable->setModel(__fir->staff());
  AirportsTable->setModel(__fir->airports());
  
  BookedATCTable->setModel(VatbookHandler::getSingleton().notNullModel(__fir->icao()));
}

void
FirDetailsWindow::__fillLabels() {
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
FirDetailsWindow::__adjustTables() {
  FlightsTable->hideColumn(FlightTableModel::Name);
  
  FlightsTable->setColumnWidth(FlightTableModel::Callsign, 150);
  FlightsTable->setColumnWidth(FlightTableModel::From, 170);
  FlightsTable->setColumnWidth(FlightTableModel::To, 170);
  FlightsTable->setColumnWidth(FlightTableModel::Aircraft, 150);

  ATCTable->setColumnWidth(ControllerTableModel::Callsign, 150);
  ATCTable->setColumnWidth(ControllerTableModel::Name, 340);
  ATCTable->setColumnWidth(ControllerTableModel::Frequency, 150);

  AirportsTable->setColumnWidth(AirportTableModel::Label, 270);
  AirportsTable->setColumnWidth(AirportTableModel::Facilities, 170);
  AirportsTable->setColumnWidth(AirportTableModel::Outbounds, 100);
  AirportsTable->setColumnWidth(AirportTableModel::Inbounds, 100);
  
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Callsign, 150);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Name, 300);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Date, 150);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Hours, 150);
}

void
FirDetailsWindow::__notamUpdate(NotamListModel* _model) {
  if (_model->icao() == __fir->icao()) {
    NotamTableView->setModel(_model);
  }
}

void
FirDetailsWindow::__goToNotam(QModelIndex _index) {
  QString url = _index.data(Qt::UserRole).toString();
  if (!url.isEmpty())
    QDesktopServices::openUrl(url);
}
