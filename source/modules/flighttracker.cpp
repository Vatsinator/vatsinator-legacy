/*
    flighttracker.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "ui/mapwidget/mapwidget.h"
#include "ui/windows/flightdetailswindow.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "flighttracker.h"
#include "defines.h"

FlightTracker::FlightTracker(QObject* _parent) :
    QObject(_parent),
    __currentCallsign(""),
    __currentPointer(NULL),
    __myMapWidget(NULL) {}

void
FlightTracker::init() {
  __myMapWidget = MapWidget::GetSingletonPtr();
  connect(__myMapWidget,  SIGNAL(flightTrackingRequested(const Pilot*)),
          this,           SLOT(__trackFlight(const Pilot*)));
  connect(__myMapWidget,  SIGNAL(flightTrackingCanceled()),
          this,           SLOT(__cancelFlight()));
  
  connect(FlightDetailsWindow::GetSingletonPtr(), SIGNAL(flightTrackingStateChanged(const Pilot*, int)),
          this,                                   SLOT(__trackFlight(const Pilot*, int)));
}

void FlightTracker::updateData() {
  if (!__currentCallsign.isEmpty()) {
    __currentPointer = VatsimDataHandler::GetSingleton().findPilot(__currentCallsign);

    if (!__currentPointer)
      __currentCallsign = "";
  }
}

void
FlightTracker::__trackFlight(const Pilot* _p) {
  Q_ASSERT(_p);
  __currentPointer = _p;
  __currentCallsign = _p->callsign;

  VatsinatorApplication::log("Tracking flight %s.", __currentCallsign.toStdString().c_str());
}

void
FlightTracker::__trackFlight(const Pilot* _p, int _state) {
  Q_ASSERT(_p);

  if (_state == Qt::Checked) {
    __trackFlight(_p);
  } else {
    if (_p == __currentPointer) {
      __cancelFlight();
    }
  }

  VatsinatorApplication::log("Tracking flight %s.", __currentCallsign.toStdString().c_str());
}

void
FlightTracker::__cancelFlight() {
  VatsinatorApplication::log("Stooped tracking flight %s.", __currentCallsign.toStdString().c_str());

  __currentCallsign = "";
  __currentPointer = NULL;
}

