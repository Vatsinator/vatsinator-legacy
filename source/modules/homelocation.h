/*
    homelocation.h
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


#ifndef HOMELOCATION_H
#define HOMELOCATION_H

#include <QObject>
#include <QPointF>

#include "singleton.h"

class HomeLocation : public QObject, public Singleton<HomeLocation> {
  
  /*
   * HomeLocation is used to store user's home location.
   * Ok, that was pretty obvious.
   */
  
  Q_OBJECT
  
public:
  HomeLocation();
  virtual ~HomeLocation();
  
  inline const QPointF &
  get() const { return __homeLocation; }
  
  inline bool
  isSet() const { return !__homeLocation.isNull(); }
  
  
public slots:
  /**
   * Gets point from MapWidget.
   */
  void set();
  
  /**
   * @param p Point.
   */
  void set(const QPointF&);
  
  /**
   * Forces MapWidget to show the homeLocation on the middle of the map.
   */
  void showOnMap();

private:
  void __store();
  
  QPointF __homeLocation;
  
};

#endif // HOMELOCATION_H
