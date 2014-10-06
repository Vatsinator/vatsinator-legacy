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
#include "ui/vatsinatorstyle.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "atcdetailswindow.h"

AtcDetailsWindow::AtcDetailsWindow(const Controller* atc, QWidget* parent) :
    QWidget(parent),
    __atc(atc) {
  setupUi(this);
  
  VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
  CallsignLabel->setFont(style->h1Font());
  FacilityLabel->setFont(style->h2Font());
  AtisMessageLabel->setFont(style->smallFont());
  
  NameLabel->setDescription(tr("Name"));
  FrequencyLabel->setDescription(tr("Frequency"));
  RatingLabel->setDescription(tr("Rating"));
  AirportLabel->setDescription(tr("Airport"));
  ServerLabel->setDescription(tr("Server"));
  TimeOnlineLabel->setDescription(tr("Online from"));
  
  connect(qApp, &QCoreApplication::aboutToQuit, this, &AtcDetailsWindow::hide);
  connect(__atc, &Controller::updated, this, &AtcDetailsWindow::__updateLabels);
  
  connect(ShowButton, &QPushButton::clicked, [this]() {
    wui()->mainWindow()->mapWidget()->renderer()->scene()->moveTo(__atc->position());
    close();
    vApp()->userInterface()->ensureMainWindowIsActive();
  });
}

void
AtcDetailsWindow::showEvent(QShowEvent* event) {
  Q_ASSERT(__atc);
  
  if (!event->spontaneous()) {
    this->setGeometry(
      QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        this->size(),
        QDesktopWidget().screenGeometry(wui()->mainWindow())
      )
    );
  }
  
  __updateLabels();
}

void
AtcDetailsWindow::__updateLabels() {
  setWindowTitle(tr("%1 - ATC details").arg(__atc->callsign()));

  CallsignLabel->setText(__atc->callsign());
  
  NameLabel->setValue(QString("%1 (%2)").arg(__atc->realName(), QString::number(__atc->pid())));
  FrequencyLabel->setValue(__atc->frequency());
  RatingLabel->setValue(Controller::ratings[__atc->rating()]);

  if (__atc->airport())
    AirportLabel->setValue(QString("%1 %2, %3").arg(
      QString(__atc->airport()->data()->icao),
      QString::fromUtf8(__atc->airport()->data()->name),
      QString::fromUtf8(__atc->airport()->data()->city)
    ));
  else
    AirportLabel->setValue(tr("N/A"));

  FacilityLabel->setText(__atc->description());
  ServerLabel->setValue(__atc->server());
  TimeOnlineLabel->setValue(__atc->onlineFrom().toString("dd MMM yyyy, hh:mm"));

  AtisMessageField->setPlainText(__atc->atis());
}
