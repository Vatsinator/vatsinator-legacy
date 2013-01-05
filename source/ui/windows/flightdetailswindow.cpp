/*
    flightdetailswindow.cpp
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

#include "modules/flighttracker.h"

#include "ui/userinterface.h"

#include "ui/widgets/mapwidget.h"
#include "ui/windows/airportdetailswindow.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsimdata/airport/activeairport.h"

#include "vatsimdata/client/pilot.h"

#include "vatsinatorapplication.h"

#include "flightdetailswindow.h"
#include "defines.h"

FlightDetailsWindow::FlightDetailsWindow(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);

  connect(TrackFlightBox, SIGNAL(stateChanged(int)), this, SLOT(stateHandle(int)));
  connect(ShowButton, SIGNAL(clicked()),  this, SLOT(__handleShowClicked()));
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(dataUpdated()),
          this,           SLOT(__updateData()));

  UserInterface::setWindowPosition(this);
}

void
FlightDetailsWindow::show(const Client* _client) {
  Q_ASSERT(dynamic_cast< const Pilot* >(_client));
  __current = dynamic_cast< const Pilot* >(_client);
  __currentCallsign = __current->getCallsign();

  if (__current->isPrefiledOnly())
    return;

  setWindowTitle(tr("%1 - flight details").arg(__current->getCallsign()));

  CallsignLabel->setText(__current->getCallsign());
  RouteLabel->setText(__current->getRoute().origin % " -> " % __current->getRoute().destination);

  PilotLabel->setText(__current->getRealName() + " (" + QString::number(__current->getPid()) + ")");
  AltitudeLabel->setText(tr("%1 feet").arg(QString::number(__current->getAltitude())));
  GroundSpeedLabel->setText(tr("%1 kts").arg(QString::number(__current->getGroundSpeed())));
  HeadingLabel->setText(QString::number(__current->getHeading()));

  if (__current->getFlightStatus() == Pilot::AIRBORNE)
    CurrentStatusLabel->setText(tr("airborne"));
  else if (__current->getFlightStatus() == Pilot::DEPARTING)
    CurrentStatusLabel->setText(tr("departing"));
  else
    CurrentStatusLabel->setText(tr("arrived"));

  ServerLabel->setText(__current->getServer());
  TimeOnlineLabel->setText(__current->getOnlineFrom().toString("dd MMM yyyy, hh:mm"));
  SquawkLabel->setText(__current->getSquawk());
  AltimeterLabel->setText(__current->getPressure().mb % " / " % __current->getPressure().ihg);
  
  VatawareLink->setText("<a href=\"http://www.vataware.com/pilot.cfm?cid=" %
      QString::number(__current->getPid()) %
      static_cast< QString >("\">") %
      tr("Vataware statistics for this pilot") %
      static_cast< QString >("</a>"));

  FlightRulesLabel->setText((__current->getFlightRules() == Pilot::IFR) ? "IFR" : "VFR");

  __updateToFromButtons();

  AircraftLabel->setText(__current->getAircraft());
  TrueAirSpeedLabel->setText(tr("%1 kts").arg(QString::number(__current->getTas())));
  CruiseAltitude->setText(__current->getRoute().altitude);

  RouteField->setPlainText(__current->getRoute().route);
  RemarksField->setPlainText(__current->getRemarks());

  if (FlightTracker::getSingleton().getTracked() == __current)
    TrackFlightBox->setCheckState(Qt::Checked);
  else
    TrackFlightBox->setCheckState(Qt::Unchecked);

  if (!isVisible())
    QWidget::show();
  else
    activateWindow();
}

void
FlightDetailsWindow::stateHandle(int _state) {
  emit flightTrackingStateChanged(__current, _state);
}

void
FlightDetailsWindow::__updateToFromButtons() {
  if (!__current->getRoute().origin.isEmpty()) {
    Airport* ap = VatsimDataHandler::getSingleton().getActiveAirports()[__current->getRoute().origin];
    QString text = __current->getRoute().origin;

    if (ap->getData()) {
      text.append(static_cast< QString >(" ") %
                  QString::fromUtf8(ap->getData()->name));

      if (!QString::fromUtf8(ap->getData()->name).contains(QString::fromUtf8(ap->getData()->city)))
        text.append(
          static_cast< QString >(" - ") %
          QString::fromUtf8(ap->getData()->city));

      OriginButton->setAirportPointer(ap);
    } else {
      OriginButton->setAirportPointer(NULL);
    }

    OriginButton->setText(text);
  } else {
    OriginButton->setText("(unknown)");
    OriginButton->setAirportPointer(NULL);
  }

  if (!__current->getRoute().destination.isEmpty()) {
    Airport* ap = VatsimDataHandler::getSingleton().getActiveAirports()[__current->getRoute().destination];
    QString text = __current->getRoute().destination;

    if (ap->getData()) {
      text.append(static_cast< QString >(" ") %
                  QString::fromUtf8(ap->getData()->name));

      if (!QString::fromUtf8(ap->getData()->name).contains(QString::fromUtf8(ap->getData()->city)))
        text.append(
          static_cast< QString >(" - ") %
          QString::fromUtf8(ap->getData()->city));

      ArrivalButton->setAirportPointer(ap);
    } else {
      ArrivalButton->setAirportPointer(NULL);
    }

    ArrivalButton->setText(text);
  } else {
    ArrivalButton->setText("(unknown)");
    ArrivalButton->setAirportPointer(NULL);
  }
}

void
FlightDetailsWindow::__updateData() {
  __current = VatsimDataHandler::getSingleton().findPilot(__currentCallsign);

  if (!__current || __current->isPrefiledOnly()) {
    __currentCallsign = "";
    hide();
  } else {
    __updateToFromButtons();
  }
}

void
FlightDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__current);
  MapWidget::getSingleton().showClient(__current);
}


