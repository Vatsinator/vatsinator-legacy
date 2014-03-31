/*
    flightdetailswindow.cpp
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

#include "db/airlinedatabase.h"
#include "db/airportdatabase.h"

#include "modules/flighttracker.h"

#include "ui/userinterface.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/airportdetailswindow.h"

#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/client/pilot.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "flightdetailswindow.h"
#include "defines.h"

FlightDetailsWindow::FlightDetailsWindow(QWidget* _parent) :
    BaseWindow(_parent) {
  setupUi(this);
  CallsignLabel->setFont(VatsinatorApplication::h1Font());
  FromLabel->setFont(VatsinatorApplication::h2Font());
  ToLabel->setFont(VatsinatorApplication::h2Font());
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));

  connect(TrackFlightBox,                       SIGNAL(stateChanged(int)),
          this,                                 SLOT(stateHandle(int)));
  connect(ShowButton,                           SIGNAL(clicked()),
          this,                                 SLOT(__handleShowClicked()));
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__updateData()));
}

void
FlightDetailsWindow::show(const Client* _client) {
  Q_ASSERT(dynamic_cast<const Pilot*>(_client));
  __current = dynamic_cast<const Pilot*>(_client);
  __currentCallsign = __current->callsign();

  if (__current->isPrefiledOnly())
    return;
  
  setWindowTitle(tr("%1 - flight details").arg(__current->callsign()));
  
  CallsignLabel->setText(__current->callsign());
  AirlineLabel->setText(AirlineDatabase::getSingleton().find(__current->callsign().left(3)));
  FromLabel->setText(__current->route().origin);
  ToLabel->setText(__current->route().destination);
  
  FlightProgress->setValue(__current->progress());
  
  if (__current->std().isValid())
    PlannedDepartureTimeLabel->setText(__current->std().toString("hh:mm"));
  else
    PlannedDepartureTimeLabel->setText("-");
  
  if (__current->atd().isValid())
    ActualDepartureTimeLabel->setText(__current->atd().toString("hh:mm"));
  else
    ActualDepartureTimeLabel->setText("-");
  
  if (__current->sta().isValid())
    PlannedArrivalTimeLabel->setText(__current->sta().toString("hh:mm"));
  else
    PlannedArrivalTimeLabel->setText("-");
  
  if (__current->eta().isValid())
    EstimatedArrivalTimeLabel->setText(__current->eta().toString("hh:mm"));
  else
    EstimatedArrivalTimeLabel->setText("-");
  
  QFont smaller;
  smaller.setPointSize(smaller.pointSize() - 2);
  StdLabel->setFont(smaller);
  StdUtcLabel->setFont(smaller);
  StaLabel->setFont(smaller);
  StaUtcLabel->setFont(smaller);
  AtdLabel->setFont(smaller);
  AtdUtcLabel->setFont(smaller);
  EtaLabel->setFont(smaller);
  EtaUtcLabel->setFont(smaller);

  PilotLabel->setText(__current->realName() + " (" + QString::number(__current->pid()) + ")");
  AltitudeLabel->setText(tr("%1 feet").arg(QString::number(__current->altitude())));
  GroundSpeedLabel->setText(tr("%1 kts").arg(QString::number(__current->groundSpeed())));
  HeadingLabel->setText(QString::number(__current->heading()));

  if (__current->flightStatus() == Pilot::AIRBORNE)
    CurrentStatusLabel->setText(tr("airborne"));
  else if (__current->flightStatus() == Pilot::DEPARTING)
    CurrentStatusLabel->setText(tr("departing"));
  else
    CurrentStatusLabel->setText(tr("arrived"));

  ServerLabel->setText(__current->server());
  TimeOnlineLabel->setText(__current->onlineFrom().toString("dd MMM yyyy, hh:mm"));
  SquawkLabel->setText(__current->squawk());
  AltimeterLabel->setText(__current->pressure().mb % " / " % __current->pressure().ihg);

  FlightRulesLabel->setText((__current->flightRules() == Pilot::IFR) ? "IFR" : "VFR");

  __updateToFromButtons();

  AircraftLabel->setText(__current->aircraft());
  TrueAirSpeedLabel->setText(tr("%1 kts").arg(QString::number(__current->tas())));
  CruiseAltitude->setText(__current->route().altitude);

  RouteField->setPlainText(__current->route().route);
  RemarksField->setPlainText(__current->remarks());

  if (FlightTracker::getSingleton().tracked() == __current)
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
  if (!__current->route().origin.isEmpty()) {
    Airport* ap = VatsimDataHandler::getSingleton().activeAirports()[__current->route().origin];
    QString text = __current->route().origin;

    if (ap->data()) {
      FromCityLabel->setText(QString::fromUtf8(ap->data()->city));
      text.append(static_cast<QString>(" ") %
                  QString::fromUtf8(ap->data()->name));

      if (!QString::fromUtf8(ap->data()->name).contains(QString::fromUtf8(ap->data()->city)))
        text.append(
          static_cast<QString>(" - ") %
          QString::fromUtf8(ap->data()->city));

      OriginButton->setAirportPointer(ap);
    } else {
      OriginButton->setAirportPointer(NULL);
      FromCityLabel->setText("");
    }

    OriginButton->setText(text);
  } else {
    OriginButton->setText("(unknown)");
    OriginButton->setAirportPointer(NULL);
    FromCityLabel->setText("");
  }

  if (!__current->route().destination.isEmpty()) {
    Airport* ap = VatsimDataHandler::getSingleton().activeAirports()[__current->route().destination];
    QString text = __current->route().destination;

    if (ap->data()) {
      ToCityLabel->setText(QString::fromUtf8(ap->data()->city));
      text.append(static_cast<QString>(" ") %
                  QString::fromUtf8(ap->data()->name));

      if (!QString::fromUtf8(ap->data()->name).contains(QString::fromUtf8(ap->data()->city)))
        text.append(
          static_cast<QString>(" - ") %
          QString::fromUtf8(ap->data()->city));

      ArrivalButton->setAirportPointer(ap);
    } else {
      ArrivalButton->setAirportPointer(NULL);
      ToCityLabel->setText("");
    }

    ArrivalButton->setText(text);
  } else {
    ArrivalButton->setText("(unknown)");
    ArrivalButton->setAirportPointer(NULL);
    ToCityLabel->setText("");
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


