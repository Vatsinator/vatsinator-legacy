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

class LonLat : public QPointF {

public:
  friend QDataStream& operator<<(QDataStream&, const LonLat&);
  friend QDataStream& operator>>(QDataStream&, LonLat&);
  
  LonLat();
  LonLat(const QPoint&);
  LonLat(const QPointF&);
  LonLat(qreal, qreal);
  
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

QDataStream& operator<<(QDataStream&, const LonLat&);
QDataStream& operator>>(QDataStream&, LonLat&);

#endif // LONLAT_H
