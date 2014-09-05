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
#include <QColor>
#include <spatial/point_multimap.hpp>
#include "vatsimdata/lonlat.h"

class QAbstractAnimation;
class QRectF;
class AirportItem;
class Controller;
class Fir;
class FirItem;
class FlightItem;
class MapItem;
class Pilot;
class MapRenderer;
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
  
  /**
   * The settings struct provides fast access to map-related settings.
   * The MapScene class makes sure these settings are always up-to-date.
   * SettingsManager::get() calls are pretty expensive, as there is always
   * some casting from QVariant involved, thus this struct keeps everything
   * here, updating it only when needed.
   */
  struct MapSettings {
    struct {
      int zoom_coefficient;
    } misc;
    
    struct {
      QColor lands;
      QColor seas;
      QColor staffed_fir_borders;
      QColor staffed_fir_background;
      QColor staffed_uir_borders;
      QColor staffed_uir_background;
      QColor unstaffed_fir_borders;
      QColor approach_circle;
    } colors;
    
    struct {
      bool airports_layer;
      bool airport_labels;
      bool pilots_layer;
      bool staffed_firs;
      bool unstaffed_firs;
      bool empty_airports;
      
      struct {
        bool always;
        bool airport_related;
        bool when_hovered;
      } pilot_labels;
    } view;
  };
  
  explicit MapScene(QObject*);
  virtual ~MapScene();
  
  void trackFlight(const Pilot*);
  void cancelFlightTracking();
  
  /**
   * Finds FirItem instance that handles the given Fir.
   * @return nullptr if no such FirItem could be found.
   */
  FirItem* findItemForFir(const Fir*);
  
  /**
   * Finds all visible items that are inside the given _rect_.
   */
  QList<const MapItem*> items(const QRectF&) const;
  
  /**
   * Finds nearest item to the given point.
   */
  const MapItem* nearest(const LonLat&);
  
  /**
   * Finds _n_ nearest items to the given point.
   */
  QList<const MapItem*> nearest(const LonLat&, int);
  
  inline MapRenderer* renderer() { return __renderer; }
  
  inline const QList<FirItem*>& firItems() const {
    return __firItems;
  }
  
  inline const QList<UirItem*>& uirItems() const {
    return __uirItems;
  }
  
  inline const Pilot* trackedFlight() const {
    return __trackedFlight;
  }
  
  inline const MapSettings& settings() const { return __settings; }
  
public slots:
  /**
   * Moves the map smoothly to the given point.
   * @param p The target point.
   */
  void moveTo(const LonLat&);
  
  /**
   * Aborts the currently running animation.
   * If no animation is being played, this function has no effect.
   */
  void abortAnimation();
  
private:
  void __addFlightItem(const Pilot*);
  
private slots:
  void __setupItems();
  void __updateItems();
  
  /**
   * This slot is connected to every Pilot's destroyed() signal.
   */
  void __removeFlightItem();
  
  /**
   * Update flight position in the tree.
   */
  void __updateFlightItem();
  
  /**
   * Updates locally-kept settings.
   */
  void __updateSettings();
  
private:
  MapRenderer* __renderer;
  
  /**
   * This map keeps FlightItems, AirportItems and FirItems.
   */
  spatial::point_multimap<2, LonLat, const MapItem*> __items;
  
  QList<FirItem*>               __firItems;
  QList<UirItem*>               __uirItems;
  
  const Pilot* __trackedFlight;
  QAbstractAnimation* __animation;
  
  MapSettings __settings;
  
};

#endif // MAPSCENE_H
