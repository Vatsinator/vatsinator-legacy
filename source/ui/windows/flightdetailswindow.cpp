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

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

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

FlightDetailsWindow::FlightDetailsWindow(const Pilot* _pilot, QWidget* _parent) :
    BaseWindow(_parent),
    __pilot(_pilot) {
  setupUi(this);
  
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
  
  VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
  
  CallsignLabel->setFont(style->h1Font());
  FromLabel->setFont(style->h2Font());
  ToLabel->setFont(style->h2Font());
  
  connect(qApp,                 SIGNAL(aboutToQuit()),
          this,                 SLOT(hide()));
  connect(TrackFlightBox,       SIGNAL(stateChanged(int)),
          this,                 SLOT(stateHandle(int)));
  connect(ShowButton,           SIGNAL(clicked()),
          this,                 SLOT(__handleClicked()));
  connect(__pilot,              SIGNAL(updated()),
          this,                 SLOT(__updateLabels()));
  connect(__pilot,              SIGNAL(invalid()),
          this,                 SLOT(close()));
}

void
FlightDetailsWindow::show() {
  Q_ASSERT(__pilot);
  
  if (__pilot->isPrefiledOnly())
    return;
  
  if (wui()->mainWindow()->mapWidget()->renderer()->scene()->trackedFlight() == __pilot)
    TrackFlightBox->setCheckState(Qt::Checked);
  else
    TrackFlightBox->setCheckState(Qt::Unchecked);
  
  __updateLabels();
  __updateButtons();
  
  BaseWindow::show();
}

void
FlightDetailsWindow::stateHandle(int _state) {
  emit flightTrackingStateChanged(__pilot, _state);
}

void
FlightDetailsWindow::__updateButtons() {
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

      ArrivalButton->setAirportPointer(ap);
    } else {
      ArrivalButton->setAirportPointer(nullptr);
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
FlightDetailsWindow::__updateLabels() {
  setWindowTitle(tr("%1 - flight details").arg(__pilot->callsign()));
  CallsignLabel->setText(__pilot->callsign());
  
  Airline* myAirline = AirlineDatabase::getSingleton().find(__pilot->callsign().left(3));
  if (myAirline) {
    QString tooltip = QString("%1 (%2)").arg(myAirline->name(), myAirline->country());
    AirlineLabel->setToolTip(tooltip);
    
    if (myAirline->logo().isNull()) {
      AirlineLabel->setText(myAirline->name());
      connect(myAirline,    SIGNAL(logoFetched()),
              this,         SLOT(__airlineUpdated()));
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

  PilotLabel->setText(QString("%1 (%2)").arg(__pilot->realName(), QString::number(__pilot->pid())));
  AltitudeLabel->setText(tr("%1 feet").arg(QString::number(__pilot->altitude())));
  GroundSpeedLabel->setText(tr("%1 kts").arg(QString::number(__pilot->groundSpeed())));
  HeadingLabel->setText(QString::number(__pilot->heading()));

  if (__pilot->phase() == Pilot::Airborne)
    CurrentStatusLabel->setText(tr("airborne"));
  else if (__pilot->phase() == Pilot::Departing)
    CurrentStatusLabel->setText(tr("departing"));
  else
    CurrentStatusLabel->setText(tr("arrived"));

  ServerLabel->setText(__pilot->server());
  TimeOnlineLabel->setText(__pilot->onlineFrom().toString("dd MMM yyyy, hh:mm"));
  SquawkLabel->setText(__pilot->squawk());
  AltimeterLabel->setText(__pilot->pressure().mb % " / " % __pilot->pressure().ihg);

  FlightRulesLabel->setText((__pilot->flightRules() == Pilot::Ifr) ? "IFR" : "VFR");

  AircraftLabel->setText(__pilot->aircraft());
  TrueAirSpeedLabel->setText(tr("%1 kts").arg(QString::number(__pilot->tas())));
  CruiseAltitude->setText(__pilot->route().altitude);

  RouteField->setPlainText(__pilot->route().route);
  RemarksField->setPlainText(__pilot->remarks());
}

void
FlightDetailsWindow::__handleClicked() {
  Q_ASSERT(__pilot);
  wui()->mainWindow()->mapWidget()->renderer()->scene()->moveSmoothly(__pilot->position());
  close();
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

