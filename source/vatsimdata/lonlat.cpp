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

#include <cmath>
#include <QtCore>
#include <QDataStream>

#include "db/point.h"

#include "lonlat.h"

namespace {
    template<class T> T mod(T x, T y)
    {
        T mod = std::fmod(x, y);
        if (mod < 0)
            mod = y + mod;
        return mod;
    }
}

LonLat::LonLat() : QPointF() {}

LonLat::LonLat(const QPoint& point): QPointF(point) {}

LonLat::LonLat(const QPointF& point): QPointF(point) {}

LonLat::LonLat(const Point& point): QPointF(point.x, point.y) {}

LonLat::LonLat(qreal longitude, qreal latitude): QPointF(longitude, latitude) {}

LonLat
LonLat::bound() const &
{
    LonLat b(*this);
    
//     b.rx() = std::fmod(b.x() + 180.0, 360.0) - 180.0;
    b.rx() = mod(b.x() + 180.0, 360.0) - 180.0;
        
//     b.ry() = qBound(-90.0, b.y(), 90.0);
    b.ry() = qBound(-85.0511, b.y(), 85.0511);
    return b;
}

LonLat
LonLat::bound() &&
{
    rx() = mod(x() + 180.0, 360.0) - 180.0;
    
//     ry() = qBound(-90.0, y(), 90.0);
    ry() = qBound(-85.0511, y(), 85.0511);
    return std::move(*this);
}

QDataStream&
operator<<(QDataStream& stream, const LonLat& lonlat)
{
    stream << lonlat.x() << lonlat.y();
    return stream;
}

QDataStream&
operator>>(QDataStream& stream, LonLat& lonlat)
{
    stream >> lonlat.rx() >> lonlat.ry();
    return stream;
}


static QVariant lonLatInterpolator(const LonLat& start, const LonLat& end, qreal progress)
{
    return LonLat(
        start.longitude() + (end.longitude() - start.longitude()) * progress,
        start.latitude() + (end.latitude() - start.latitude()) * progress
    );
}

static void registerType()
{
    qRegisterMetaType<LonLat>("LonLat");
    qRegisterMetaTypeStreamOperators<LonLat>("LonLat");
    qRegisterAnimationInterpolator<LonLat>(&lonLatInterpolator);
}
Q_COREAPP_STARTUP_FUNCTION(registerType)
