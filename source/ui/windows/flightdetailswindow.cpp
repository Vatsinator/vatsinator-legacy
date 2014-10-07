/*
    flightdetailswindow.cpp
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

#include "db/airline.h"
#include "db/airlinedatabase.h"
#include "db/airportdatabase.h"
#include "ui/userinterface.h"
#include "ui/vatsinatorstyle.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/client/pilot.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "flightdetailswindow.h"

FlightDetailsWindow::FlightDetailsWindow(const Pilot* pilot, QWidget* parent) :
    QWidget(parent),
    __pilot(pilot) {
  setupUi(this);
  
  VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
  
  /* Setup fonts fonts */
  QFont smaller = style->smallFont();
  StdLabel->setFont(smaller);
  StdUtcLabel->setFont(smaller);
  StaLabel->setFont(smaller);
  StaUtcLabel->setFont(smaller);
  AtdLabel->setFont(smaller);
  AtdUtcLabel->setFont(smaller);
  EtaLabel->setFont(smaller);
  EtaUtcLabel->setFont(smaller);
  
  OriginLabel->setFont(smaller);
  DestinationLabel->setFont(smaller);
  RouteLabel->setFont(smaller);
  RemarksLabel->setFont(smaller);
  
  CallsignLabel->setFont(style->h1Font());
  FromLabel->setFont(style->h2Font());
  ToLabel->setFont(style->h2Font());
  
  PilotLabel->setDescription(tr("Pilot"));
  AltitudeLabel->setDescription(tr("Altitude"));
  GroundSpeedLabel->setDescription(tr("Ground speed"));
  HeadingLabel->setDescription(tr("Heading"));
  FlightPhaseLabel->setDescription(tr("Flight phase"));
  ServerLabel->setDescription(tr("Server"));
  TimeOnlineLabel->setDescription(tr("Online from"));
  SquawkLabel->setDescription(tr("Squawk"));
  AltimeterLabel->setDescription(tr("Baro"));
  FlightRulesLabel->setDescription(tr("Flight rules"));
  AircraftLabel->setDescription(tr("Aircraft"));
  TrueAirSpeedLabel->setDescription(tr("TAS"));
  CruiseAltitudeLabel->setDescription(tr("Cruise altitude"));
  
  connect(qApp, &QCoreApplication::aboutToQuit, this, &FlightDetailsWindow::hide);
  connect(__pilot, &Pilot::updated, this, &FlightDetailsWindow::__updateInfo);
  connect(__pilot, &Pilot::invalid, this, &FlightDetailsWindow::close);
  
  connect(OriginButton,                 SIGNAL(clicked(const Airport*)),
          vApp()->userInterface(),      SLOT(showDetails(const Airport*)));
  connect(DestinationButton,            SIGNAL(clicked(const Airport*)),
          vApp()->userInterface(),      SLOT(showDetails(const Airport*)));
  
  connect(ShowButton, &QPushButton::clicked, [this]() {
    wui()->mainWindow()->mapWidget()->renderer()->scene()->moveTo(__pilot->position());
    close();
    vApp()->userInterface()->ensureMainWindowIsActive();
  });
  
  connect(TrackFlightBox, &QCheckBox::stateChanged, [this](int _state) {
    emit flightTrackingStateChanged(__pilot, _state);
  });
}

void
FlightDetailsWindow::showEvent(QShowEvent* event) {
  Q_ASSERT(__pilot);
  
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
  
  if (wui()->mainWindow()->mapWidget()->renderer()->scene()->trackedFlight() == __pilot)
    TrackFlightBox->setCheckState(Qt::Checked);
  else
    TrackFlightBox->setCheckState(Qt::Unchecked);
  
  __updateInfo();
  __updateAirports();
}

void
FlightDetailsWindow::__updateAirports() {
  if (!__pilot->route().origin.isEmpty()) {
    const Airport* ap = __pilot->origin();
    QString text = __pilot->route().origin;

    if (ap) {
      FromCityLabel->setText(QString::fromUtf8(ap->data()->city));
      text.append(static_cast<QString>(" ") %
                  QString::fromUtf8(ap->data()->name));

      if (!QString::fromUtf8(ap->data()->name).contains(QString::fromUtf8(ap->data()->city)))
        text.append(
          static_cast<QString>(" - ") %
          QString::fromUtf8(ap->data()->city));

      OriginButton->setAirportPointer(ap);
    } else {
      OriginButton->setAirportPointer(nullptr);
      FromCityLabel->setText("");
    }

    OriginButton->setText(text);
  } else {
    OriginButton->setText("(unknown)");
    OriginButton->setAirportPointer(nullptr);
    FromCityLabel->setText("");
  }

  if (!__pilot->route().destination.isEmpty()) {
    const Airport* ap = __pilot->destination();
    QString text = __pilot->route().destination;

    if (ap) {
      ToCityLabel->setText(QString::fromUtf8(ap->data()->city));
      text.append(static_cast<QString>(" ") %
                  QString::fromUtf8(ap->data()->name));

      if (!QString::fromUtf8(ap->data()->name).contains(QString::fromUtf8(ap->data()->city)))
        text.append(
          static_cast<QString>(" - ") %
          QString::fromUtf8(ap->data()->city));

      DestinationButton->setAirportPointer(ap);
    } else {
      DestinationButton->setAirportPointer(nullptr);
      ToCityLabel->setText("");
    }

    DestinationButton->setText(text);
  } else {
    DestinationButton->setText("(unknown)");
    DestinationButton->setAirportPointer(NULL);
    ToCityLabel->setText("");
  }
}

void
FlightDetailsWindow::__updateInfo() {
  setWindowTitle(tr("%1 - flight details").arg(__pilot->callsign()));
  CallsignLabel->setText(__pilot->callsign());
  
  Airline* myAirline = vApp()->airlineDatabase()->find(__pilot->callsign().left(3));
  if (myAirline) {
    QString tooltip = QString("%1 (%2)").arg(myAirline->name(), myAirline->country());
    AirlineLabel->setToolTip(tooltip);
    
    if (myAirline->logo().isNull()) {
      AirlineLabel->setText(myAirline->name());
      connect(myAirline, &Airline::logoFetched, this, &FlightDetailsWindow::__airlineUpdated);
      myAirline->requestLogo();
    } else {
      AirlineLabel->setPixmap(QPixmap::fromImage(myAirline->logo()));
    }
  } else {
    AirlineLabel->setPixmap(QPixmap());
    AirlineLabel->setText("");
    AirlineLabel->setToolTip("");
  }
  
  FromLabel->setText(__pilot->route().origin);
  ToLabel->setText(__pilot->route().destination);
  
  FlightProgress->setValue(__pilot->progress());
  
  PlannedDepartureTimeLabel->setText(__pilot->std().isValid() ? __pilot->std().toString("hh:mm") : "-");
  ActualDepartureTimeLabel->setText(__pilot->atd().isValid() ? __pilot->atd().toString("hh:mm") : "-");
  PlannedArrivalTimeLabel->setText(__pilot->sta().isValid() ? __pilot->sta().toString("hh:mm") : "-");
  EstimatedArrivalTimeLabel->setText(__pilot->eta().isValid() ? __pilot->eta().toString("hh:mm") : "-");

  PilotLabel->setValue(QString("%1 (%2)").arg(__pilot->realName(), QString::number(__pilot->pid())));
  AltitudeLabel->setValue(tr("%1 feet").arg(QString::number(__pilot->altitude())));
  GroundSpeedLabel->setValue(tr("%1 kts").arg(QString::number(__pilot->groundSpeed())));
  HeadingLabel->setValue(QString::number(__pilot->heading()));

  if (__pilot->phase() == Pilot::Airborne)
    FlightPhaseLabel->setValue(tr("airborne"));
  else if (__pilot->phase() == Pilot::Departing)
    FlightPhaseLabel->setValue(tr("departing"));
  else
    FlightPhaseLabel->setValue(tr("arrived"));

  ServerLabel->setValue(__pilot->server());
  TimeOnlineLabel->setValue(__pilot->onlineFrom().toString("dd MMM yyyy, hh:mm"));
  SquawkLabel->setValue(__pilot->squawk());
  AltimeterLabel->setValue(__pilot->pressure().mb % " / " % __pilot->pressure().ihg);

  FlightRulesLabel->setValue((__pilot->flightRules() == Pilot::Ifr) ? "IFR" : "VFR");
  AircraftLabel->setValue(__pilot->aircraft());
  TrueAirSpeedLabel->setValue(tr("%1 kts").arg(QString::number(__pilot->tas())));
  CruiseAltitudeLabel->setValue(__pilot->route().altitude);

  RouteField->setPlainText(__pilot->route().route);
  RemarksField->setPlainText(__pilot->remarks());
}

void
FlightDetailsWindow::__airlineUpdated() {
  Airline* a = qobject_cast<Airline*>(sender());
  Q_CHECK_PTR(a);
  
  const QImage& logo = a->logo();
  if (!logo.isNull()) {
    AirlineLabel->setPixmap(QPixmap::fromImage(logo));
  }
}

