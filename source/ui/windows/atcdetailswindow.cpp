/*
    atcdetailswindow.cpp
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
#include "db/firdatabase.h"
#include "ui/userinterface.h"
#include "ui/widgets/mapwidget.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "atcdetailswindow.h"
#include "defines.h"

AtcDetailsWindow::AtcDetailsWindow(QWidget* _parent) :
    BaseWindow(_parent) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));

  connect(ShowButton,                           SIGNAL(clicked()),
          this,                                 SLOT(__handleShowClicked()));
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__updateData()));
}

void
AtcDetailsWindow::show(const Client* _client) {
  Q_ASSERT(dynamic_cast<const Controller*>(_client));
  __current = dynamic_cast<const Controller*>(_client);
  __currentCallsign = __current->callsign();

  setWindowTitle(tr("%1 - ATC details").arg(__current->callsign()));

  CallsignLabel->setText(__current->callsign());
  FacilityLabel->setText(__current->description());
  NameLabel->setText(__current->realName() + " (" + QString::number(__current->pid()) + ")");
  FrequencyLabel->setText(__current->frequency());
  RatingLabel->setText(Controller::ratings[__current->rating()]);

  if (__current->airport())
    AirportLabel->setText(static_cast<QString>(__current->airport()->data()->icao) %
                          " " %
                          QString::fromUtf8(__current->airport()->data()->name) %
                          ", " %
                          QString::fromUtf8(__current->airport()->data()->city)
                         );
  else
    AirportLabel->setText(tr("N/A"));

  ServerLabel->setText(__current->server());
  TimeOnlineLabel->setText(__current->onlineFrom().toString("dd MMM yyyy, hh:mm"));

  AtisMessageField->setPlainText(__current->atis());

  if (!isVisible())
    QWidget::show();
  else
    activateWindow();
}

void
AtcDetailsWindow::__updateData() {
  __current = VatsimDataHandler::getSingleton().findAtc(__currentCallsign);
  
  if (!__current) {
    __currentCallsign = "";
    hide();
  }
}

void
AtcDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__current);
  // TODO
//   MapWidget::getSingleton().showClient(__current);
}

