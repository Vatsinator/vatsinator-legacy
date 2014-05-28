/*
 * mapscene.h
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

#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QObject>
#include <QList>

class AirportItem;
class ApproachCircleItem;
class FirItem;
class FlightItem;
class Pilot;

class MapScene : public QObject {
  
  /**
   * The MapScene class is responsible for managing all the map items.
   */
  
  Q_OBJECT

public:
  explicit MapScene(QObject* parent = 0);
  virtual ~MapScene();
  
  inline const QList<FirItem*>& firItems() const {
    return __firItems;
  }
  
  inline const QList<AirportItem*>& airportItems() const {
    return __airportItems;
  }
    
  inline const QList<ApproachCircleItem*>& approachCircleItems() const {
    return __approachCircleItems;
  }
  
  inline const QList<FlightItem*>& flightItems() const {
    return __flightItems;
  }
  
private:
  void __setupItems();
  void __addFlightItem(const Pilot*);
  
private slots:
  void __updateItems();
  
  /**
   * This slot is connected to every Pilot's destroyed() signal.
   */
  void __removeFlightItem();
  
private:
  
  QList<FirItem*>               __firItems;
  QList<AirportItem*>           __airportItems;
  QList<ApproachCircleItem*>    __approachCircleItems;
  QList<FlightItem*>            __flightItems;
  
};

#endif // MAPSCENE_H
