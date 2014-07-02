/*
    homelocation.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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

#include "ui/widgets/mapwidget.h"

#include "homelocation.h"
#include "defines.h"

HomeLocation::HomeLocation() :
    QObject() {
  QSettings s;
  s.beginGroup("Modules");
  
  __homeLocation = s.value("homelocation").toPointF();
  
  s.endGroup();
}

HomeLocation::~HomeLocation() {
  __store();
}

void
HomeLocation::set() {
//   qreal lat, lon;
//   MapWidget::getSingleton().mouse2LatLon(&lat, &lon);
//   __homeLocation.setX(lon);
//   __homeLocation.setY(lat);
}

void
HomeLocation::set(const QPointF& _p) {
  __homeLocation = _p;
}

void
HomeLocation::showOnMap() {
//   MapWidget::getSingleton().showPoint(__homeLocation);
}

void
HomeLocation::__store() {
  if (__homeLocation.isNull())
    return;
  
  QSettings s;
  s.beginGroup("Modules");
  
  s.setValue("homelocation", __homeLocation);
  
  s.endGroup();
}
