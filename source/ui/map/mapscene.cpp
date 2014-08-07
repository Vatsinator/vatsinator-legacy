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
#include "events/mapevent.h"
#include "ui/map/abstractanimation.h"
#include "ui/map/airportitem.h"
#include "ui/map/approachcircleitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/moveanimation.h"
#include "ui/map/uiritem.h"
#include "ui/widgets/mapwidget.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "mapscene.h"

MapScene::MapScene(QObject* parent) :
    QObject(parent),
    __trackedFlight(nullptr),
    __animation(nullptr) {
  
  __widget = qobject_cast<MapWidget*>(parent);
  Q_ASSERT(__widget);
  
  connect(vApp()->vatsimDataHandler(),  SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(__updateItems()));
  connect(vApp()->vatsimDataHandler(),  SIGNAL(initialized()),
          this,                         SLOT(__setupItems()));
}

MapScene::~MapScene() {}

void
MapScene::trackFlight(const Pilot* _p) {
  __trackedFlight = _p;
  emit flightTracked(__trackedFlight);
}

void
MapScene::cancelFlightTracking() {
  __trackedFlight = nullptr;
  emit flightTracked(__trackedFlight);
}

void
MapScene::startAnimation(AbstractAnimation* _animation) {
  if (__animation) {
    __animation->abort();
    __animation->deleteLater();
  }
  
  __animation = _animation;
  
  __animation->setParent(this);
  __animation->setPosition(__widget->state().center());
  __animation->setZoom(__widget->state().zoom());
  connect(__animation,  SIGNAL(step()),
          this,         SLOT(__animationStep()));
  connect(__animation,  SIGNAL(stopped()),
          this,         SLOT(__animationDestroy()));
  __animation->start();
}

FirItem*
MapScene::findItemForFir(const Fir* _fir) {
  for (FirItem* f: firItems())
    if (f->data() == _fir)
      return f;
  
  return nullptr;
}

QList<const MapItem*>
MapScene::items(const QRectF& _rect) const {
  QList<const MapItem*> result;
  for (const FlightItem* f: __flightItems) {
    if (_rect.contains(f->position()))
      result << f;
  }
  
  return result;
}

void
MapScene::moveSmoothly(const LonLat& _target) {
  MoveAnimation* a = new MoveAnimation(this);
  a->setTarget(_target);
  startAnimation(a);
}

void
MapScene::__addFlightItem(const Pilot* _p) {
  connect(_p,           SIGNAL(destroyed(QObject*)),
          this,         SLOT(__removeFlightItem()), Qt::DirectConnection);
  __flightItems << new FlightItem(_p, this);
}

void
MapScene::__setupItems() {
  for (const Airport* a: vApp()->vatsimDataHandler()->airports()) {
    __airportItems << new AirportItem(a, this);
  }
  
  for (const Fir* f: vApp()->vatsimDataHandler()->firs()) {
    if (f->data()->header.textPosition.x != 0.0 && f->data()->header.textPosition.y != 0.0) {
      __firItems << new FirItem(f, this);
    }
  }
  
  for (const Uir* u: vApp()->vatsimDataHandler()->uirs()) {
    __uirItems << new UirItem(u, this);
  }
  
  for (auto c: vApp()->vatsimDataHandler()->clients())
    if (Pilot* p = dynamic_cast<Pilot*>(c)) {
      if (p->phase() != Pilot::Arrived)
        __addFlightItem(p);
    }
}

void
MapScene::__updateItems() {
  for (Client* c: vApp()->vatsimDataHandler()->newClients())
    if (Pilot* p = dynamic_cast<Pilot*>(c)) {
      if (p->phase() != Pilot::Arrived)
        __addFlightItem(p);
    }
}

void
MapScene::__animationStep() {
  Q_ASSERT(__animation);
  
  MapState state(__widget->state());
  
  if (!(__animation->flags() & AbstractAnimation::NoPositionOverride))
    state.setCenter(__animation->position());
  
  if (!(__animation->flags() & AbstractAnimation::NoZoomOverride))
    state.setZoom(__animation->zoom());
  
  MapEvent e(state);
  bool accepted = qApp->notify(__widget, &e);
  if (!accepted) {
    __animation->abort();
    __animation->deleteLater();
    __animation = nullptr;
  }
}

void
MapScene::__animationDestroy() {
  __animation->deleteLater();
  __animation = nullptr;
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
