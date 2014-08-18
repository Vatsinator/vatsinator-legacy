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

#include <QtWidgets>

#include "db/airportdatabase.h"
#include "modules/vatbookhandler.h"
#include "modules/models/bookedatctablemodel.h"
#include "network/abstractnotamprovider.h"
#include "plugins/weatherforecastinterface.h"
#include "plugins/weatherforecastrequest.h"
#include "plugins/weatherforecastreply.h"
#include "storage/settingsmanager.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "ui/buttons/clientdetailsbutton.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/widgets/mapwidget.h"
#include "ui/widgets/weatherforecastwidget.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/client.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "airportdetailswindow.h"

AirportDetailsWindow::AirportDetailsWindow(const Airport* _ap, QWidget* _parent) :
    BaseWindow(_parent),
    __airport(_ap) {
  setupUi(this);
  
  connect(qApp,                                 SIGNAL(aboutToQuit()),
          this,                                 SLOT(hide()));
  connect(ShowButton,                           SIGNAL(clicked()),
          this,                                 SLOT(__handleShowClicked()));
  connect(vApp()->vatsimDataHandler()->notamProvider(),
                                                SIGNAL(notamReady(NotamListModel*)),
          this,                                 SLOT(__notamUpdate(NotamListModel*)));
  connect(NotamTableView,                       SIGNAL(doubleClicked(QModelIndex)),
          this,                                 SLOT(__goToNotam(QModelIndex)));
  
  NotamTableView->setErrorOnNoData(false);
  WeatherForecastScrollArea->setWidget(new WeatherForecastWidget());
}

AirportDetailsWindow::~AirportDetailsWindow() {}

void
AirportDetailsWindow::showEvent(QShowEvent* _event) {
  __fillLabels();
  __updateModels();
  __adjustTables();
  
  WeatherForecastWidget* w = qobject_cast<WeatherForecastWidget*>(WeatherForecastScrollArea->widget());
  if (vApp()->vatsimDataHandler()->weatherForecast()) {
    ForecastGroup->setEnabled(true);
    
    WeatherForecastRequest* request = new WeatherForecastRequest(__airport->icao());
    request->setCity(QString::fromUtf8(__airport->data()->city));
    request->setCountry(QString::fromUtf8(__airport->data()->country));
    request->setPosition(__airport->position());
    
    WeatherForecastReply* reply = vApp()->vatsimDataHandler()->weatherForecast()->fetch(request);
    connect(reply,      SIGNAL(finished()),
            this,       SLOT(__updateForecast()));
    
    if (SM::get("network.weather_temperature_units").toString() == "Celsius")
      w->setCelsius();
    else
      w->setFahrenheit();
  } else {
    ForecastGroup->setEnabled(false);
    w->setMessage(tr("No plugin selected"));
  }
  
  NotamTableView->setModel(nullptr);
  vApp()->vatsimDataHandler()->notamProvider()->fetchNotam(QString(__airport->data()->icao));
  NotamProviderInfoLabel->setText(vApp()->vatsimDataHandler()->notamProvider()->providerInfo());
  
  BaseWindow::showEvent(_event);
}

void
AirportDetailsWindow::__updateModels() {
  InboundTable->setModel(__airport->inbounds());
  OutboundTable->setModel(__airport->outbounds());
  ATCTable->setModel(__airport->staff());
  
  BookedATCTable->setModel(
      VatbookHandler::getSingleton().notNullModel(QString::fromUtf8(__airport->data()->icao)));
}

void
AirportDetailsWindow::__fillLabels() {
  setWindowTitle(tr("%1 - airport details").arg(__airport->data()->icao));

  if (!QString(__airport->data()->iata).isEmpty())
    CodesLabel->setText(QString("%1 / %2").arg(QString(__airport->data()->icao),
                                               QString(__airport->data()->iata)));
  else
    CodesLabel->setText(QString(__airport->data()->icao));

  NameLabel->setText(QString("%1, %2").arg(QString::fromUtf8(__airport->data()->name),
                                           QString::fromUtf8(__airport->data()->city)));

  // fill "Airport info" tab
  FullNameLabel->setText(QString::fromUtf8(__airport->data()->name));
  CityLabel->setText(QString::fromUtf8(__airport->data()->city));
  CountryLabel->setText(QString::fromUtf8(__airport->data()->country));
  AltitudeLabel->setText(tr("%1 ft").arg(QString::number(__airport->data()->altitude)));
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
AirportDetailsWindow::__updateForecast() {
  WeatherForecastReply* r = qobject_cast<WeatherForecastReply*>(sender());
  Q_ASSERT(r);
  
  WeatherForecastWidget* w = qobject_cast<WeatherForecastWidget*>(WeatherForecastScrollArea->widget());
  Q_ASSERT(w);
  
  if (r->error() == WeatherForecastReply::NoError) {
    w->setData(r->data());
  } else {
    switch (r->error()) {
      case WeatherForecastReply::NotFoundError:
        w->setMessage(tr("No forecast for %1, %2").arg(r->request()->country(), r->request()->city()));
        break;
        
      case WeatherForecastReply::NetworkError:
        w->setMessage(tr("Network error"));
        break;
    }
  }
}

void
AirportDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__airport);
  wui()->mainWindow()->mapWidget()->renderer()->scene()->moveSmoothly(__airport->position());
  close();
}

void
AirportDetailsWindow::__notamUpdate(NotamListModel* _model) {
  if (_model->icao() == QString(__airport->data()->icao))
    NotamTableView->setModel(_model);
}

void
AirportDetailsWindow::__goToNotam(QModelIndex _index) {
  QString url = _index.data(Qt::UserRole).toString();
  if (!url.isEmpty())
    QDesktopServices::openUrl(url);
}
