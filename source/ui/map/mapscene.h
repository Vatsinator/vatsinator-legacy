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

class QRectF;
class AbstractAnimation;
class AirportItem;
class Controller;
class Fir;
class FirItem;
class FlightItem;
class MapItem;
class LonLat;
class Pilot;
class MapWidget;
class UirItem;

/**
 * The MapScene class is responsible for managing all the map items, moving
 * the map around (except situations when user moves it using mouse) and
 * zooming it appropiately.
 */
class MapScene : public QObject {
  Q_OBJECT
  
signals:
  
  /**
   * Emitted when user wants to track the flight.
   * If the pointer is nullptr, it means user just cancelled flight tracking.
   */
  void flightTracked(const Pilot*);

public:
  explicit MapScene(QObject* parent);
  virtual ~MapScene();
  
  void trackFlight(const Pilot*);
  void cancelFlightTracking();
  
  /**
   * Starts the given animation, aborting any currently running one.
   * This function takes ownership over the given animation.
   * NOTE: The animation can't be started yet. This function call start()
   * on the instance.
   * @param animation The map animation instance.
   */
  void startAnimation(AbstractAnimation*);
  
  /**
   * Finds FirItem instance that handles the given Fir.
   * @return nullptr if no such FirItem could be found.
   */
  FirItem* findItemForFir(const Fir*);
  
  /**
   * Finds all visible items that are inside the given _rect_.
   */
  QList<const MapItem*> items(const QRectF&) const;
  
  inline const QList<FirItem*>& firItems() const {
    return __firItems;
  }
  
  inline const QList<AirportItem*>& airportItems() const {
    return __airportItems;
  }
  
  inline const QList<FlightItem*>& flightItems() const {
    return __flightItems;
  }
  
  inline const QList<UirItem*>& uirItems() const {
    return __uirItems;
  }
  
  inline const Pilot* trackedFlight() const {
    return __trackedFlight;
  }
  
  inline AbstractAnimation* animation() {
    return __animation;
  }
  
public slots:
  /**
   * Moves the map smoothly to the given point.
   * @param p The target point.
   */
  void moveSmoothly(const LonLat&);
  
private:
  void __addFlightItem(const Pilot*);
  
private slots:
  void __setupItems();
  void __updateItems();
  void __animationStep();
  void __animationDestroy();
  
  /**
   * This slot is connected to every Pilot's destroyed() signal.
   */
  void __removeFlightItem();
  
private:
  MapWidget*    __widget;
  
  QList<FirItem*>               __firItems;
  QList<AirportItem*>           __airportItems;
  QList<FlightItem*>            __flightItems;
  QList<UirItem*>               __uirItems;
  
  const Pilot* __trackedFlight;
  
  AbstractAnimation*    __animation;
  
};

#endif // MAPSCENE_H
