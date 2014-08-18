/*
 * mapconfig.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#ifndef MAPCONFIG_H
#define MAPCONFIG_H

#include <QtGlobal> // qreal
#include <QGLFormat> // glFormat

class QColor;
class QFont;
class QPixmap;

class MapConfig {
public:
  
  static constexpr qreal longitudeMax() {
    return 180.0;
  }
  
  static constexpr qreal latitudeMax() {
    return 90.0;
  }
  
  static constexpr int baseWindowWidth() {
    return 800;
  }
  
  static constexpr int baseWindowHeight() {
    return 600;
  }
  
  /* Distance between mouse and object that object is considered
     to be under mouse, in pixels. */
  static constexpr qreal mouseOnObject() {
    return 15.0;
  }
  
  /* User won't be able to zoom below this value */
  static constexpr int zoomMinimum() {
    return 1;
  }
  
  /* User won't be able to zoom above this value */
  static constexpr int zoomMaximum() {
    return 1500;
  }
  
  static constexpr qreal zoomNormalizeCoef() {
    return 0.5;
  }
  
  static constexpr qreal zoomBase() {
    return 1.1;
  }
  
  static const QGLFormat& glFormat();
  
  static const QString& emptyAirportIcon();
  static const QString& activeAirportIcon();
  static const QString& activeStaffedAirportIcon();
  
  static const QColor& pilotPen();
  static const QColor& airportPen();
  
  static const QImage& pilotLabelBackground();
  static const QImage& airportLabelBackground();
  static const QImage& firLabelBackground();
  
  static const QFont& pilotFont();
  static const QFont& airportFont();
  static const QFont& firFont();
  
  /* Map layers */
  enum MapLayers {
    WorldMap = 1,
    UnstaffedFirs = 2,
    StaffedUirs = 3,
    StaffedFirs = 4,
    
    EmptyAirports = 6,
    ActiveAirports = 7,
    
    Lines = 9,
    
    Pilots = 11,
    
    Count
  };
  
  MapConfig() = delete;
  
};

#endif // MAPCONFIG_H
