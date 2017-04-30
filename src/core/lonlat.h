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

#ifndef CORE_LONLAT_H
#define CORE_LONLAT_H

#include "core/vtrcore_export.h"
#include <QtCore/QPointF>
#include <QtCore/QMetaType>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The LonLat class is a wrapper around QPointF to provide convenient
 * handling global coordinates - latitude and longitude.
 */
class VTRCORE_EXPORT LonLat : public QPointF {

public:
    /**
     * The default constructor creates point (0.0, 0.0).
     */
    LonLat();
    
    /**
     * Creates new LonLat from the given point.
     */
    explicit LonLat(const QPoint& point);
    
    /**
     * Creates new LonLat from the given point.
     */
    LonLat(const QPointF& point);
    
    /**
     * Creates new LonLat from the given coordinates - \c longitude
     * and \c latitude.
     */
    LonLat(qreal longitude, qreal latitude);
    
    /**
     * Returns this point, but bounds its coordinates to be correct longitude
     * and latitude.
     */
    LonLat bound() const &;
    
    /**
     * Returns this point, but bounds its coordinates to be correct longitude
     * and latitude.
     */
    LonLat bound() &&;
    
    /**
     * Returns \c longitude of this point.
     */
    qreal longitude() const { return QPointF::x(); }
    
    /**
     * Returns \c latitude of this point.
     */
    qreal latitude() const { return QPointF::y(); }
    
    /**
     * Converts JSON-encoded LonLat object and returns it.
     */
    static LonLat fromJson(const QJsonObject& json);
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::LonLat)

VTRCORE_EXPORT QDataStream& operator<<(QDataStream& stream, const Vatsinator::Core::LonLat& lonlat);
VTRCORE_EXPORT QDataStream& operator>>(QDataStream& stream, Vatsinator::Core::LonLat& lonlat);

#endif // CORE_LONLAT_H
