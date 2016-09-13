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

#include "lonlat.h"
#include <QtCore>
#include <QDataStream>

namespace Vatsinator { namespace Core {

LonLat::LonLat() : QPointF() {}

LonLat::LonLat(const QPoint& point): QPointF(point) {}

LonLat::LonLat(const QPointF& point): QPointF(point) {}

LonLat::LonLat(qreal longitude, qreal latitude): QPointF(longitude, latitude) {}

LonLat
LonLat::bound() const &
{
    LonLat b(*this);
    
    while (b.x() > 180.0)
        b.rx() -= 360.0;
        
    while (b.x() < -180.0)
        b.rx() += 360.0;
        
//     b.ry() = qBound(-90.0, b.y(), 90.0);
    b.ry() = qBound(-85.0511, b.y(), 85.0511);
    return b;
}

LonLat
LonLat::bound() &&
{
    while (x() > 180.0)
        rx() -= 360.0;
    
    while (x() < -180.0)
        rx() += 360.0;
    
//     ry() = qBound(-90.0, y(), 90.0);
    ry() = qBound(-85.0511, y(), 85.0511);
    return std::move(*this);
}

LonLat LonLat::fromJson(const QJsonObject& json)
{
    LonLat ll;
    if (json.value("longitude").isDouble())
        ll.setX(json.value("longitude").toDouble());
    
    if (json.value("latitude").isDouble())
        ll.setY(json.value("latitude").toDouble());
    
    return ll;
}

}} /* namespace Vatsinator::Core */

QDataStream&
operator<<(QDataStream& stream, const Vatsinator::Core::LonLat& lonlat)
{
    stream << lonlat.x() << lonlat.y();
    return stream;
}

QDataStream&
operator>>(QDataStream& stream, Vatsinator::Core::LonLat& lonlat)
{
    stream >> lonlat.rx() >> lonlat.ry();
    return stream;
}

static QVariant lonLatInterpolator(const Vatsinator::Core::LonLat& start,
                                   const Vatsinator::Core::LonLat& end,
                                   qreal progress)
{
    return QVariant::fromValue(Vatsinator::Core::LonLat(
        start.longitude() + (end.longitude() - start.longitude()) * progress,
        start.latitude() + (end.latitude() - start.latitude()) * progress
    ));
}

static void registerType()
{
    qRegisterMetaType<Vatsinator::Core::LonLat>();
    qRegisterMetaTypeStreamOperators<Vatsinator::Core::LonLat>();
    qRegisterAnimationInterpolator<Vatsinator::Core::LonLat>(&lonLatInterpolator);
}
Q_COREAPP_STARTUP_FUNCTION(registerType)
