/*
    airportdetailswindow.cpp
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
#include "network/metarupdater.h"
#include "network/weatherforecastdownloader.h"
#include "plugins/atcbookingtablemodel.h"
#include "plugins/bookingprovider.h"
#include "plugins/notamprovider.h"
#include "plugins/weatherforecastrequest.h"
#include "plugins/weatherforecastreply.h"
#include "storage/settingsmanager.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "ui/buttons/clientdetailsbutton.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/models/metarlistmodel.h"
#include "ui/models/roles.h"
#include "ui/widgets/mapwidget.h"
#include "ui/widgets/weatherforecastwidget.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "ui/vatsinatorstyle.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/client.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/metar.h"
#include "vatsimdata/pilot.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "airportdetailswindow.h"

AirportDetailsWindow::AirportDetailsWindow(const Airport* airport, QWidget* parent) :
    BaseWindow(parent),
    __airport(airport) {
  setupUi(this);
  
  VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
  CodesLabel->setFont(style->h1Font());
  
  InboundTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  OutboundTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  AtcTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  BookedATCTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  
  connect(qApp, &QCoreApplication::aboutToQuit, this, &AirportDetailsWindow::close);
  connect(InboundTable, &QTableView::doubleClicked, this, &AirportDetailsWindow::__showDetails);
  connect(OutboundTable, &QTableView::doubleClicked, this, &AirportDetailsWindow::__showDetails);
  connect(AtcTable, &QTableView::doubleClicked, this, &AirportDetailsWindow::__showDetails);
  connect(NotamTableView, &DelayedModelTableView::doubleClicked,
          this, &AirportDetailsWindow::__goToNotam);
  connect(vApp()->vatsimDataHandler()->notamProvider(), &NotamProvider::notamReady,
          this, &AirportDetailsWindow::__updateNotam);
  
  connect(ShowButton, &QPushButton::clicked, [this]() {
    wui()->mainWindow()->mapWidget()->renderer()->scene()->cancelFlightTracking();
    wui()->mainWindow()->mapWidget()->renderer()->scene()->moveTo(__airport->position());
    close();
    vApp()->userInterface()->ensureMainWindowIsActive();
  });
  
  NotamTableView->setErrorOnNoData(false);
   
  WeatherForecastWidget* w = new WeatherForecastWidget();
  
  /* Animation */
  QMovie* animation = new QMovie(":/animations/loader.gif");
  w->setAnimation(animation);
  WeatherForecastScrollArea->setWidget(w);
}

AirportDetailsWindow::~AirportDetailsWindow() {}

void
AirportDetailsWindow::showEvent(QShowEvent* event) {
  __fillLabels();
  
  /* Set models */
  InboundTable->setModel(__airport->inbounds());
  OutboundTable->setModel(__airport->outbounds());
  AtcTable->setModel(__airport->staff());
  BookedATCTable->setModel(vApp()->vatsimDataHandler()->bookingProvider()->bookings(__airport->icao()));
  
  /* Hide obvious things */
  InboundTable->hideColumn(FlightTableModel::Name);
  InboundTable->hideColumn(FlightTableModel::To);
  OutboundTable->hideColumn(FlightTableModel::Name);
  OutboundTable->hideColumn(FlightTableModel::From);
  
  WeatherForecastWidget* w = qobject_cast<WeatherForecastWidget*>(WeatherForecastScrollArea->widget());
  if (SM::get("network.weather_temperature_units").toString() == "Celsius")
    w->setCelsius();
  else
    w->setFahrenheit();
  
  WeatherForecastDownloader* wfd = new WeatherForecastDownloader(this);
  connect(wfd, &WeatherForecastDownloader::finished, this, &AirportDetailsWindow::__updateForecast);
  
  WeatherForecastRequest* request = new WeatherForecastRequest(__airport->icao());
  request->setCity(QString::fromUtf8(__airport->data()->city));
  request->setCountry(QString::fromUtf8(__airport->data()->country));
  request->setPosition(__airport->position());
  
  wfd->download(request);
  
  NotamTableView->setModel(nullptr);
  vApp()->vatsimDataHandler()->notamProvider()->fetchNotam(__airport->icao());  
  NotamProviderInfoLabel->setText(vApp()->vatsimDataHandler()->notamProvider()->providerInfo());
  
  BaseWindow::showEvent(event);
}

void
AirportDetailsWindow::__fillLabels() {
  setWindowTitle(tr("%1 - airport details").arg(__airport->icao()));

  if (!QString(__airport->data()->iata).isEmpty()) {
    CodesLabel->setText(QString("%1 / %2").arg(QString(__airport->data()->icao),
                                               QString(__airport->data()->iata)));
  } else {
    CodesLabel->setText(QString(__airport->data()->icao));
  }

  NameLabel->setText(QString("%1, %2").arg(QString::fromUtf8(__airport->data()->name),
                                           QString::fromUtf8(__airport->data()->city)));

  // fill "Airport info" tab
  FullNameLabel->setText(QString::fromUtf8(__airport->data()->name));
  CityLabel->setText(QString::fromUtf8(__airport->data()->city));
  CountryLabel->setText(QString::fromUtf8(__airport->data()->country));
  AltitudeLabel->setText(tr("%1 ft").arg(QString::number(__airport->data()->altitude)));
  
  // If METAR is available, set it. Otherwise, request it.
  auto matches = vApp()->metarUpdater()->model()->match(
      vApp()->metarUpdater()->model()->index(0), MetarRole, __airport->icao(), 1);
  if (matches.length() > 0) {
    Metar metar = matches.first().data(MetarRole).value<Metar>();
    MetarLabel->setText(metar.metar());
  } else {
    vApp()->metarUpdater()->fetch(__airport->icao());
  }
  
  connect(vApp()->metarUpdater()->model(), &MetarListModel::dataChanged, this, &AirportDetailsWindow::__metarUpdated);
  connect(vApp()->metarUpdater()->model(), &MetarListModel::rowsInserted, this, &AirportDetailsWindow::__metarUpdated);
}

void
AirportDetailsWindow::__updateForecast(WeatherForecastReply* reply) {
  WeatherForecastWidget* w = qobject_cast<WeatherForecastWidget*>(WeatherForecastScrollArea->widget());
  Q_ASSERT(w);
  w->setStatus(DelayedWidget::Finished);
  
  if (reply->error() == WeatherForecastReply::NoError)
    w->setData(reply->data());
  else
    w->setMessage(tr("Error"));
}

void
AirportDetailsWindow::__updateNotam(NotamListModel* model) {
  if (model->icao() == __airport->icao())
    NotamTableView->setModel(model);
}

void
AirportDetailsWindow::__showDetails(QModelIndex index) {
  Q_ASSERT(index.data(InstancePointerRole).isValid());
  Client* const client = reinterpret_cast<Client* const>(index.data(InstancePointerRole).value<void*>());
  vApp()->userInterface()->showDetails(client);
}

void
AirportDetailsWindow::__goToNotam(QModelIndex index) {
  QString url = index.data(UrlRole).toString();
  if (!url.isEmpty())
    QDesktopServices::openUrl(url);
}

void
AirportDetailsWindow::__metarUpdated() {
  auto matches = vApp()->metarUpdater()->model()->match(
      vApp()->metarUpdater()->model()->index(0), MetarRole, __airport->icao(), 1);
  
  if (matches.length() > 0) {
    Metar metar = matches.first().data(MetarRole).value<Metar>();
    if (metar.metar() != MetarLabel->text())
      MetarLabel->setText(metar.metar());
  }
}
