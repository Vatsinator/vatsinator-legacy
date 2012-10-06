/*
    atcdetailswindow.cpp
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
#include "db/firdatabase.h"

#include "ui/userinterface.h"

#include "ui/widgets/mapwidget.h"

#include "vatsimdata/controller.h"

#include "vatsinatorapplication.h"

#include "atcdetailswindow.h"
#include "defines.h"

ATCDetailsWindow::ATCDetailsWindow(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);
  UserInterface::setWindowPosition(this);

  connect(ShowButton, SIGNAL(clicked()), this, SLOT(__handleShowClicked()));
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(dataUpdated()),
          this,       SLOT(__updateData()));
}

void
ATCDetailsWindow::show(const Client* _client) {
  Q_ASSERT(dynamic_cast< const Controller* >(_client));
  __current = dynamic_cast< const Controller* >(_client);
  __currentCallsign = __current->callsign;

  setWindowTitle(QString(__current->callsign + " - " + tr("ATC details")));

  CallsignLabel->setText(__current->callsign);
  FacilityLabel->setText(__current->description);
  NameLabel->setText(__current->realName + " (" + QString::number(__current->pid) + ")");
  FrequencyLabel->setText(__current->frequency);
  RatingLabel->setText(Controller::ratings[__current->rating]);

  if (__current->airport)
    AirportLabel->setText(static_cast< QString >(__current->airport->icao) %
                          " " %
                          QString::fromUtf8(__current->airport->name) %
                          ", " %
                          QString::fromUtf8(__current->airport->city)
                         );
  else
    AirportLabel->setText(tr("N/A"));

  ServerLabel->setText(__current->server);
  TimeOnlineLabel->setText(__current->onlineFrom.toString("dd MMM yyyy, hh:mm"));

  AtisMessageField->setPlainText(__current->atis);

  if (!isVisible())
    QWidget::show();
  else
    activateWindow();
}

void
ATCDetailsWindow::__updateData() {
  __current = VatsimDataHandler::getSingleton().findATC(__currentCallsign);
  
  if (!__current) {
    __currentCallsign = "";
    hide();
  }
}

void
ATCDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__current);
  MapWidget::getSingleton().showClient(__current);
}

