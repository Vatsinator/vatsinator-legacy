/*
    airporttracker.cpp
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

#include <QtWidgets>

#include "db/airportdatabase.h"

#include "ui/widgets/mapwidget.h"

#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "airporttracker.h"

AirportTracker::AirportTracker(QObject* _parent) :
    QObject(_parent),
    __isInitialized(false) {
  connect(MapWidget::getSingletonPtr(), SIGNAL(airportLinesToggled(const Airport*)),
          this,                         SLOT(__toggleAirport(const Airport*)));
}

AirportTracker::~AirportTracker() {
  QStringList trackedAirports(__trackedAirports.keys());
  
  QSettings settings;
  settings.beginGroup("Modules");
  
  settings.setValue("trackedAirports", trackedAirports.join(":"));
  
  settings.endGroup();
}

void
AirportTracker::updateData() {
  if (!__isInitialized)
    __readSettings();
  
  for (auto it = __trackedAirports.begin(); it != __trackedAirports.end(); ++it) {
    it.value() = VatsimDataHandler::getSingleton().findAirport(it.key());
  }
}

void
AirportTracker::__readSettings() {
  QSettings settings("Vatsinator", "Vatsinator");
  settings.beginGroup("Modules");
  
  QString trackedAirports = settings.value("trackedAirports").toString();
  
  settings.endGroup();
  
  for (QString& icao: trackedAirports.split(':')) {
    if (!icao.isEmpty())
      __trackedAirports.insert(icao, NULL);
  }
  
  __isInitialized = true;
}

void
AirportTracker::__toggleAirport(const Airport* _ap) {
  QString icao(_ap->data()->icao);
  if (__trackedAirports.contains(icao))
    __trackedAirports.remove(icao);
  else
    __trackedAirports.insert(icao, _ap);
  
  VatsinatorApplication::log("Airpor toggled: %s", qPrintable(icao));
}

