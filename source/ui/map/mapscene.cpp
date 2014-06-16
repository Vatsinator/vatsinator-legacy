/*
 * mapscene.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtCore>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "ui/map/airportitem.h"
#include "ui/map/approachcircleitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "mapscene.h"
#include "defines.h"

MapScene::MapScene(QObject* parent): QObject(parent) {
  
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__updateItems()));
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(initialized()),
          this,                                 SLOT(__setupItems()));
}

void
MapScene::__addFlightItem(const Pilot* _p) {
  connect(_p,           SIGNAL(destroyed(QObject*)),
          this,         SLOT(__removeFlightItem()), Qt::DirectConnection);
  __flightItems << new FlightItem(_p, this);
}

void
MapScene::__setupItems() {
  for (const Airport* a: VatsimDataHandler::getSingleton().airports()) {
    __airportItems << new AirportItem(a, this);
  }
  
  for (const Fir* f: VatsimDataHandler::getSingleton().firs()) {
    if (f->data()->header.textPosition.x != 0.0 && f->data()->header.textPosition.y != 0.0) {
      __firItems << new FirItem(f, this);
    }
  }
  
  for (auto c: VatsimDataHandler::getSingleton().clients())
    if (Pilot* p = dynamic_cast<Pilot*>(c)) {
      if (p->phase() != Pilot::Arrived)
        __addFlightItem(p);
    } else if (Controller* a = dynamic_cast<Controller*>(c)) {
      if (a->facility() == Controller::App)
        __approachCircleItems << new ApproachCircleItem(a->airport(), this);
    }
}

void
MapScene::__updateItems() {
  for (Client* c: VatsimDataHandler::getSingleton().newClients())
    if (Pilot* p = dynamic_cast<Pilot*>(c)) {
      if (p->phase() != Pilot::Arrived)
        __addFlightItem(p);
    } else if (Controller* a = dynamic_cast<Controller*>(c)) {
      if (a->facility() == Controller::App)
        __approachCircleItems << new ApproachCircleItem(a->airport(), this);
    }
}

void
MapScene::__removeFlightItem() {
  Q_ASSERT(sender());
  for (int i = 0; i < __flightItems.size(); ++i) {
    if (__flightItems[i]->data() == sender()) {
      __flightItems.at(i)->deleteLater();
      __flightItems.removeAt(i);
      
      return;
    }
  }
  
  Q_ASSERT_X(false, "MapScene", "The flight does not exist in the scene");
}
