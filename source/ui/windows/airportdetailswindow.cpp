/*
    airportdetailswindow.cpp
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

#include "db/airportdatabase.h"
#include "modules/vatbookhandler.h"
#include "modules/models/bookedatctablemodel.h"
#include "network/abstractnotamprovider.h"
#include "network/weatherforecast.h"
#include "network/models/weatherforecastmodel.h"
#include "storage/settingsmanager.h"
#include "ui/userinterface.h"
#include "ui/buttons/clientdetailsbutton.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/client.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/models/metarlistmodel.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "airportdetailswindow.h"
#include "defines.h"

AirportDetailsWindow::AirportDetailsWindow(QWidget* _parent) :
    BaseWindow(_parent),
    __currentICAO(""),
    __forecast(new WeatherForecast()) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));
  connect(MetarListModel::getSingletonPtr(),    SIGNAL(newMetarsAvailable()),
          this,                                 SLOT(updateMetar()));
  connect(MetarListModel::getSingletonPtr(),    SIGNAL(noMetar(QString)),
          this,                                 SLOT(updateMetar(QString)));
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__updateData()));
  connect(ShowButton,                           SIGNAL(clicked()),
          this,                                 SLOT(__handleShowClicked()));
  connect(__forecast,                           SIGNAL(forecastReady(WeatherForecastModel*)),
          this,                                 SLOT(__updateForecast(WeatherForecastModel*)));
  connect(VatsimDataHandler::getSingletonPtr()->notamProvider(),
                                                SIGNAL(notamReady(NotamListModel*)),
          this,                                 SLOT(__notamUpdate(NotamListModel*)));
  connect(NotamTableView,                       SIGNAL(doubleClicked(QModelIndex)),
          this,                                 SLOT(__goToNotam(QModelIndex)));
  
  ForecastView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);;
  ForecastView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  NotamTableView->setErrorOnNoData(false);
}

AirportDetailsWindow::~AirportDetailsWindow() {
  delete __forecast;
  
  QAbstractItemModel* m = ForecastView->model();
  if (m)
    delete m;
}

void
AirportDetailsWindow::show(const Airport* _ap) {
  Q_ASSERT(_ap->data());

  __currentICAO = _ap->data()->icao;
  __current = _ap;

  __fillLabels(_ap);
  __updateModels(_ap);
  __adjustTables();

  const Metar* m = MetarListModel::getSingleton().find(__currentICAO);

  if (m) {
    MetarLabel->setText(m->metar());
  } else {
    MetarLabel->setText(tr("Fetching..."));
    MetarListModel::getSingleton().fetchMetar(__currentICAO);
  }

  if (!isVisible()) {
    QWidget::show();
  } else {
    activateWindow();
  }

  QAbstractItemModel* fvm = ForecastView->model();
  if (fvm)
    fvm->deleteLater();
  
  if (SM::get("network.weather_forecasts").toBool()) {
    ForecastGroup->setEnabled(true);
    ForecastView->setModel(nullptr);
    __forecast->fetchForecast(QString::fromUtf8(_ap->data()->city),
                              QString::fromUtf8(_ap->data()->country));
  } else {
    ForecastGroup->setEnabled(false);
  }
  
  NotamTableView->setModel(nullptr);
  VatsimDataHandler::getSingleton().notamProvider()->fetchNotam(__currentICAO);
  NotamProviderInfoLabel->setText(VatsimDataHandler::getSingleton().notamProvider()->providerInfo());
}

void
AirportDetailsWindow::updateMetar() {
  if (__currentICAO.isEmpty())
    return;

  const Metar* m = MetarListModel::getSingleton().find(__currentICAO);

  if (m)
    MetarLabel->setText(m->metar());
}

void
AirportDetailsWindow::updateMetar(QString _icao) {
  if (__currentICAO.isEmpty() || !isVisible())
    return;
  
  if (_icao == __currentICAO)
    MetarLabel->setText(tr("Sorry, no weather report for %1.").arg(__currentICAO));
}

void
AirportDetailsWindow::__updateModels(const Airport* _ap) {
  if (!_ap)
    _ap = VatsimDataHandler::getSingleton().findAirport(__currentICAO);
  
  Q_ASSERT(_ap);
  
  __current = _ap;
  
  if (__current->isStaffed()) {
    InboundTable->setModel(__current->inbounds());
    OutboundTable->setModel(__current->outbounds());
    ATCTable->setModel(__current->staff());
  } else {
    InboundTable->setModel(VatsimDataHandler::emptyFlightTable);
    OutboundTable->setModel(VatsimDataHandler::emptyFlightTable);
    ATCTable->setModel(VatsimDataHandler::emptyControllerTable);
  }
  
  BookedATCTable->setModel(
      VatbookHandler::getSingleton().getNotNullModel(QString::fromUtf8(_ap->data()->icao)));
}

void
AirportDetailsWindow::__fillLabels(const Airport* _ap) {
  setWindowTitle(tr("%1 - airport details").arg(_ap->data()->icao));

  if (!static_cast<QString>(_ap->data()->iata).isEmpty())
    CodesLabel->setText(
      static_cast<QString>(_ap->data()->icao) %
      "/" %
      static_cast<QString>(_ap->data()->iata)
    );
  else
    CodesLabel->setText(static_cast<QString>(_ap->data()->icao));

  NameLabel->setText(
    QString::fromUtf8(_ap->data()->name) %
    ", " %
    QString::fromUtf8(_ap->data()->city)

#ifndef NO_DEBUG
    //display FIR info only in debug mode
    % " (" %
    static_cast<QString>(_ap->data()->fir_a) % " & " %
    static_cast<QString>(_ap->data()->fir_b) % " FIR)"
#endif
  );

  // fill "Airport info" tab
  const AirportRecord* apData = _ap->data();
  FullNameLabel->setText(QString::fromUtf8(apData->name));
  CityLabel->setText(QString::fromUtf8(apData->city));
  CountryLabel->setText(QString::fromUtf8(apData->country));
  AltitudeLabel->setText(tr("%1 ft").arg(QString::number(apData->altitude)));
}

void
AirportDetailsWindow::__adjustTables() {
  // make the table nice
  InboundTable->hideColumn(FlightTableModel::Name);
  InboundTable->hideColumn(FlightTableModel::To);

  InboundTable->setColumnWidth(FlightTableModel::Callsign, 150);
  InboundTable->setColumnWidth(FlightTableModel::From, 350);
  InboundTable->setColumnWidth(FlightTableModel::Aircraft, 150);

  OutboundTable->hideColumn(FlightTableModel::Name);
  OutboundTable->hideColumn(FlightTableModel::From);

  OutboundTable->setColumnWidth(FlightTableModel::Callsign, 150);
  OutboundTable->setColumnWidth(FlightTableModel::To, 350);
  OutboundTable->setColumnWidth(FlightTableModel::Aircraft, 150);

  ATCTable->setColumnWidth(ControllerTableModel::Callsign, 150);
  ATCTable->setColumnWidth(ControllerTableModel::Name, 350);
  ATCTable->setColumnWidth(ControllerTableModel::Frequency, 150);
  
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Callsign, 150);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Name, 300);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Date, 150);
  BookedATCTable->setColumnWidth(BookedAtcTableModel::Hours, 150);
}

void
AirportDetailsWindow::__updateData() {
  if (__currentICAO.isEmpty() || !isVisible())
    return;

  __updateModels();
  __adjustTables();
}

void
AirportDetailsWindow::__updateForecast(WeatherForecastModel* model) {
  QAbstractItemModel* m = ForecastView->model();
  if (m)
    m->deleteLater();
  
  ForecastView->setModel(model);
}

void
AirportDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__current);
  // TODO
//   MapWidget::getSingleton().showAirport(__current);
}

void
AirportDetailsWindow::__notamUpdate(NotamListModel* _model) {
  if (isVisible() && _model->icao() == __currentICAO) {
    NotamTableView->setModel(_model);
  }
}

void
AirportDetailsWindow::__goToNotam(QModelIndex _index) {
  QString url = _index.data(Qt::UserRole).toString();
  if (!url.isEmpty())
    QDesktopServices::openUrl(url);
}
