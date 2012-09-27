/*
    airportdetailswindow.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include "db/airportdatabase.h"

#include "modules/vatbookhandler.h"

#include "modules/models/bookedatctablemodel.h"

#include "ui/userinterface.h"

#include "ui/buttons/clientdetailsbutton.h"

#include "ui/widgets/mapwidget.h"

#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"

#include "vatsimdata/airport.h"
#include "vatsimdata/client.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/metarlistmodel.h"

#include "vatsinatorapplication.h"

#include "airportdetailswindow.h"
#include "defines.h"

AirportDetailsWindow::AirportDetailsWindow(QWidget* _parent) :
    QWidget(_parent),
    __currentICAO("") {
  setupUi(this);
  UserInterface::setWindowPosition(this);

  connect(MetarListModel::GetSingletonPtr(),  SIGNAL(newMetarsAvailable()),
          this,                               SLOT(updateMetar()));
  
  connect(MetarListModel::GetSingletonPtr(),  SIGNAL(noMetar(QString)),
          this,                               SLOT(updateMetar(QString)));

  connect(VatsinatorApplication::GetSingletonPtr(), SIGNAL(dataUpdated()),
          this,                                     SLOT(__updateData()));

  connect(ShowButton, SIGNAL(clicked()),
          this,       SLOT(__handleShowClicked()));
}

void
AirportDetailsWindow::show(const Airport* _ap) {
  Q_ASSERT(_ap->getData());

  __currentICAO = _ap->getData()->icao;
  __current = _ap;

  __fillLabels(_ap);
  __updateModels(_ap);
  
  if (dynamic_cast< const ActiveAirport* >(_ap)) {
    __setInboundTableButtons();
    __setOutboundTableButtons();
    __setATCTableButtons();
  }
  __adjustTables();

  const Metar* m = MetarListModel::GetSingleton().find(__currentICAO);

  if (m) {
    MetarLabel->setText(m->getMetar());
  } else {
    MetarLabel->setText(tr("Fetching..."));
    MetarListModel::GetSingleton().fetchMetar(__currentICAO);
  }

  if (!isVisible())
    QWidget::show();
  else
    activateWindow();
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
AirportDetailsWindow::updateMetar(QString _icao) {
  if (__currentICAO.isEmpty() || !isVisible())
    return;
  
  if (_icao == __currentICAO)
    MetarLabel->setText(tr("Sorry, no weather report for") % " " % __currentICAO % ".");
}

void
AirportDetailsWindow::__updateModels(const Airport* _ap) {
  if (!_ap)
    _ap = VatsimDataHandler::GetSingleton().findAirport(__currentICAO);
  
  Q_ASSERT(_ap);
  
  __current = _ap;
  
  disconnect(this, SLOT(__setInboundTableButtons()));
  disconnect(this, SLOT(__setOutboundTableButtons()));
  disconnect(this, SLOT(__setATCTableButtons()));
  
  const ActiveAirport* aa = dynamic_cast< const ActiveAirport* >(_ap);
  if (aa) {
    connect(aa->getInboundsModel(), SIGNAL(sorted()),
            this,                   SLOT(__setInboundTableButtons()));
    InboundTable->setModel(aa->getInboundsModel());
    
    connect(aa->getOutboundsModel(),  SIGNAL(sorted()),
            this,                     SLOT(__setOutboundTableButtons()));
    OutboundTable->setModel(aa->getOutboundsModel());
    
    connect(aa->getStaffModel(),  SIGNAL(sorted()),
            this,                 SLOT(__setATCTableButtons()));
    ATCTable->setModel(aa->getStaffModel());
  }
  
  BookedATCTable->setModel(
      VatbookHandler::GetSingleton().getModel(QString::fromUtf8(_ap->getData()->icao)));
}

void
AirportDetailsWindow::__fillLabels(const Airport* _ap) {
  setWindowTitle(static_cast< QString >(_ap->getData()->icao) + " - " + tr("airport details"));

  if (!static_cast< QString >(_ap->getData()->iata).isEmpty())
    CodesLabel->setText(
      static_cast< QString >(_ap->getData()->icao) %
      "/" %
      static_cast< QString >(_ap->getData()->iata)
    );
  else
    CodesLabel->setText(static_cast< QString >(_ap->getData()->icao));

  NameLabel->setText(
    QString::fromUtf8(_ap->getData()->name) %
    ", " %
    QString::fromUtf8(_ap->getData()->city)

#ifndef NO_DEBUG
    //display FIR info only in debug mode
    % " (" %
    static_cast< QString >(_ap->getData()->fir_a) % " & " %
    static_cast< QString >(_ap->getData()->fir_b) % " FIR)"
#endif
  );

  // fill "Airport info" tab
  const AirportRecord* apData = _ap->getData();
  FullNameLabel->setText(QString::fromUtf8(apData->name));
  CityLabel->setText(QString::fromUtf8(apData->city));
  CountryLabel->setText(QString::fromUtf8(apData->country));
  AltitudeLabel->setText(QString::number(apData->altitude) + " " + tr(" ft"));
  VatawareAirportLinkLabel->setText("<a href=\"http://www.vataware.com/airport.cfm?airport=" %
      QString::fromUtf8(apData->icao) %
      static_cast< QString >("\">") %
      tr("Vataware statistics for this airport") %
      static_cast< QString >("</a>")
    );
}

void
AirportDetailsWindow::__adjustTables() {
  // make the table nice
  InboundTable->hideColumn(FlightTableModel::Name);
  InboundTable->hideColumn(FlightTableModel::To);

  InboundTable->setColumnWidth(FlightTableModel::Callsign, 150);
  InboundTable->setColumnWidth(FlightTableModel::From, 320);
  InboundTable->setColumnWidth(FlightTableModel::Aircraft, 150);

  OutboundTable->hideColumn(FlightTableModel::Name);
  OutboundTable->hideColumn(FlightTableModel::From);

  OutboundTable->setColumnWidth(FlightTableModel::Callsign, 150);
  OutboundTable->setColumnWidth(FlightTableModel::To, 320);
  OutboundTable->setColumnWidth(FlightTableModel::Aircraft, 150);

  ATCTable->setColumnWidth(ControllerTableModel::Callsign, 150);
  ATCTable->setColumnWidth(ControllerTableModel::Name, 320);
  ATCTable->setColumnWidth(ControllerTableModel::Frequency, 150);
  
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Callsign, 150);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Name, 300);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Date, 150);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Hours, 150);
}

void
AirportDetailsWindow::__setInboundTableButtons() {
  if (!InboundTable->model())
    return;
  
  const FlightTableModel* inboundModel = qobject_cast< const FlightTableModel* >(InboundTable->model());
  Q_ASSERT(inboundModel);
  
  for (int i = 0; i < inboundModel->rowCount(); ++i) {
    if (inboundModel->getFlights()[i]->prefiledOnly)
      continue;
    
    ClientDetailsButton* pButton = new ClientDetailsButton(inboundModel->getFlights()[i]);
    connect(pButton,        SIGNAL(clicked(const Client*)),
            FlightDetailsWindow::GetSingletonPtr(), SLOT(show(const Client*)));
    InboundTable->setIndexWidget(inboundModel->index(i, FlightTableModel::Button), pButton);
  }
}

void
AirportDetailsWindow::__setOutboundTableButtons() {
  if (!OutboundTable->model())
    return;
  
  const FlightTableModel* outboundModel = qobject_cast< const FlightTableModel* >(OutboundTable->model());
  
  Q_ASSERT(outboundModel);
  
  for (int i = 0; i < outboundModel->rowCount(); ++i) {
    if (outboundModel->getFlights()[i]->prefiledOnly)
      continue;
    
    ClientDetailsButton* pButton = new ClientDetailsButton(outboundModel->getFlights()[i]);
    connect(pButton,        SIGNAL(clicked(const Client*)),
            FlightDetailsWindow::GetSingletonPtr(), SLOT(show(const Client*)));
    OutboundTable->setIndexWidget(outboundModel->index(i, FlightTableModel::Button), pButton);
  }
}

void
AirportDetailsWindow::__setATCTableButtons() {
  if (!ATCTable->model())
    return;
  
  const ControllerTableModel* atcModel = qobject_cast< const ControllerTableModel* >(ATCTable->model());
  
  Q_ASSERT(atcModel);
  
  for (int i = 0; i < atcModel->rowCount(); ++i) {
    ClientDetailsButton* pButton = new ClientDetailsButton(atcModel->getStaff()[i]);
    connect(pButton,        SIGNAL(clicked(const Client*)),
            ATCDetailsWindow::GetSingletonPtr(),  SLOT(show(const Client*)));
    ATCTable->setIndexWidget(atcModel->index(i, ControllerTableModel::Button), pButton);
  }
}

void
AirportDetailsWindow::__updateData() {
  if (__currentICAO.isEmpty() || !isVisible())
    return;

  __updateModels();
  __setInboundTableButtons();
  __setOutboundTableButtons();
  __setATCTableButtons();
  __adjustTables();
}

void
AirportDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__current);
  MapWidget::GetSingleton().showAirport(__current);
}
