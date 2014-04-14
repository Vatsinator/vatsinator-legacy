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
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "mapscene.h"
#include "defines.h"

MapScene::MapScene(QObject* parent): QObject(parent) {
  for (const Fir& f: FirDatabase::getSingleton().firs())
    __firItems << new FirItem(&f);
  
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__updateData()));
  
  __initData();
}

MapScene::~MapScene() {
  qDeleteAll(__firItems);
  qDeleteAll(__airportItems);
  qDeleteAll(__approachCircleItems);
  qDeleteAll(__flightItems);
}

void
MapScene::__initData() {
  
}

void
MapScene::__updateData() {
  /**
   * TODO
   * 
   * This is wrong approach, as it removes all the objects and creates
   * new ones. Instead, these objects should be only updated, old ones
   * removed and only these new ones added.
   */
  
  qDeleteAll(__approachCircleItems), __approachCircleItems.clear();
  /*
  for (AirportRecord& ap: AirportDatabase::getSingleton().airports()) {
    if (VatsimDataHandler::getSingleton().activeAirports().contains(ap.icao)) {
      __activeAirportItems << new AirportItem(VatsimDataHandler::getSingleton().activeAirports()[ap.icao]);
      if (VatsimDataHandler::getSingleton().activeAirports()[ap.icao]->hasApproach())
        __approachCircleItems << new ApproachCircleItem(VatsimDataHandler::getSingleton().activeAirports()[ap.icao]);
    } else {
      __emptyAirportItems << new AirportItem(VatsimDataHandler::getSingleton().addEmptyAirport(&ap));
    }
  }*/
  
  qDeleteAll(__flightItems), __flightItems.clear();
  
  for (const Pilot* p: VatsimDataHandler::getSingleton().flights()->flights()) {
    __flightItems << new FlightItem(p);
  }
}
