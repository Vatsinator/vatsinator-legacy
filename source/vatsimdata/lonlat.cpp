/*
 * lonlat.cpp
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

#include <QtGlobal>
#include <QDataStream>

#include "lonlat.h"

LonLat::LonLat() : QPointF() {}

LonLat::LonLat(const QPoint& _p): QPointF(_p) {}

LonLat::LonLat(const QPointF& _p): QPointF(_p) {}

LonLat::LonLat(qreal _lon, qreal _lat): QPointF(_lon, _lat) {}

LonLat
LonLat::bound() const {
  LonLat b(*this);
  while (b.x() > 180.0)
    b.rx() -= 360.0;
  while (b.x() < -180.0)
    b.rx() += 360.0;
  
  b.ry() = qBound(-90.0, b.y(), 90.0);
  return qMove(b);
}

QDataStream &
operator<<(QDataStream& _stream, const LonLat& _lonlat) {
  _stream << _lonlat.x() << _lonlat.y();
  return _stream;
}

QDataStream &
operator>>(QDataStream& _stream, LonLat& _lonlat) {
  _stream >> _lonlat.rx() >> _lonlat.ry();
  return _stream;
}
