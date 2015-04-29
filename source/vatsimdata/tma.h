/*
 * tma.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef TMA_H
#define TMA_H

#include <QObject>
#include <QVector>
#include "db/point.h"

class Airport;

/**
 * The Tma class represents a single TMA. It is mainly used by the map
 * to show its boundaries.
 */
class Tma : public QObject {
  Q_OBJECT
  
  /**
   * Stores the TMA's ICAO.
   */
  Q_PROPERTY(QString icao READ icao)
  
  /**
   * This property tells whether the TMA is triangulated or not.
   */
  Q_PROPERTY(bool triangulated READ isTriangulated NOTIFY triangulated)
  
signals:
  /**
   * Emitted when the triangulation is finished.
   */
  void triangulated();
  
public:
  Tma(QString icao, QObject* parent = nullptr);
  Tma(QString icao, const QJsonArray& coords, QObject* parent = nullptr);
  
  /**
   * Triangulates the TMA.
   */
  void triangulate();
  
  /**
   * Returns _new_ TMA that is a default circle around the _airport_.
   * 
   * This special TMA is used whether the required one cannot be found.
   * 
   * \param airport An airport to create the circle around.
   */
  static Tma* circle(const Airport* airport);
  
  /**
   * Gives direct access to points used to construct the TMA area.
   */
  inline const QVector<Point>& points() const { return __points; }
  
  /**
   * Gives direct access to triangles.
   * \note Triangles are valid only if **triangulated** is _true_.
   */
  inline const QVector<quint16>& triangles() const { return __triangles; }
  
  inline const QString& icao() const { return __icao; }
  inline bool isTriangulated() const { return __triangulated; }
  
private:
  void __load(const QJsonArray& coords);
  
  QString __icao;
  bool __triangulated;
  
  QVector<Point> __points;
  QVector<quint16> __triangles;
  
};

#endif // TMA_H
