/*
 * lonlat.h
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

#ifndef LONLAT_H
#define LONLAT_H

#include <QPointF>
#include <QMetaType>

/**
 * The LonLat class is a wrapper around QPointF to provide convenience
 * handling global coordinates - latitude and longitude.
 */
class LonLat : public QPointF {

public:
  friend QDataStream& operator<<(QDataStream&, const LonLat&);
  friend QDataStream& operator>>(QDataStream&, LonLat&);
  
  /**
   * The default constructor creates point (0.0, 0.0).
   */
  LonLat();
  
  /**
   * Creates new LonLat from the given point.
   */
  LonLat(const QPoint& point);
  
  /**
   * Creates new LonLat from the given point.
   */
  LonLat(const QPointF& point);
  
  /**
   * Creates new LonLat from the given coordinates - _longitude_
   * and _latitude_.
   */
  LonLat(qreal longitude, qreal latitude);
  
  /**
   * Returns this point, but bounds its coordinates to be correct longitude
   * and latitude.
   */
  LonLat bound() const;
  
  inline qreal longitude() const { return QPointF::x(); }
  inline qreal latitude() const { return QPointF::y(); }
  
  /**
   * Returns value by dimension.
   * Needed for spatial library.
   */
  inline qreal operator [](int dim) const {
    Q_ASSERT(dim >= 0 && dim <= 1);
    return dim == 0 ? x() : y();
  } 

};
Q_DECLARE_METATYPE(LonLat)

QDataStream& operator<<(QDataStream& stream, const LonLat& lonlat);
QDataStream& operator>>(QDataStream& stream, LonLat& lonlat);

#endif // LONLAT_H
