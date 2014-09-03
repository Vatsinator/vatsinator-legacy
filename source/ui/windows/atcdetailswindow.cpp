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

#include <QtWidgets>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "ui/userinterface.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "atcdetailswindow.h"

AtcDetailsWindow::AtcDetailsWindow(const Controller* _c, QWidget* _parent) :
    BaseWindow(_parent),
    __atc(_c) {
  setupUi(this);
  
  connect(qApp,                 SIGNAL(aboutToQuit()),
          this,                 SLOT(hide()));
  connect(ShowButton,           SIGNAL(clicked()),
          this,                 SLOT(__handleClicked()));
  connect(__atc,                SIGNAL(updated()),
          this,                 SLOT(__updateLabels()));
}

void
AtcDetailsWindow::show() {
  Q_ASSERT(__atc);
  __updateLabels();
  BaseWindow::show();
}

void
AtcDetailsWindow::__updateLabels() {
  setWindowTitle(tr("%1 - ATC details").arg(__atc->callsign()));

  CallsignLabel->setText(__atc->callsign());
  FacilityLabel->setText(__atc->description());
  NameLabel->setText(QString("%1 (%2)").arg(__atc->realName(), QString::number(__atc->pid())));
  FrequencyLabel->setText(__atc->frequency());
  RatingLabel->setText(Controller::ratings[__atc->rating()]);

  if (__atc->airport())
    AirportLabel->setText(QString("%1 %2, %3").arg(
      QString(__atc->airport()->data()->icao),
      QString::fromUtf8(__atc->airport()->data()->name),
      QString::fromUtf8(__atc->airport()->data()->city)
    ));
  else
    AirportLabel->setText(tr("N/A"));

  ServerLabel->setText(__atc->server());
  TimeOnlineLabel->setText(__atc->onlineFrom().toString("dd MMM yyyy, hh:mm"));

  AtisMessageField->setPlainText(__atc->atis());
}

void
AtcDetailsWindow::__handleClicked() {
  Q_ASSERT(__atc);
  wui()->mainWindow()->mapWidget()->renderer()->scene()->moveTo(__atc->position());
  close();
}

