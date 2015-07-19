/*
 * worldtransform.h
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

#ifndef WORLDTRANSFORM_H
#define WORLDTRANSFORM_H

#include <QPoint>
#include <QSize>
#include "vatsimdata/lonlat.h"

/**
 * The WorldTransform class represents a transformation between
 * global coordinate system (longitude/latitude) and window one, used
 * by the QPainter.
 */
class WorldTransform {

public:
    WorldTransform(const QSize& viewport, const LonLat& offset, qreal scale);
    
    /**
     * Maps the given geo position to screen coordinates.
     */
    QPoint map(const LonLat& lonLat) const;
    
    inline const QSize& viewport() const
    {
        return __viewport;
    }
    
    inline const LonLat& offset() const
    {
        return __offset;
    }
    
    inline qreal scale() const
    {
        return __scale;
    }
    
private:
    QSize __viewport;
    LonLat __offset;
    qreal __scale;
    
};

/**
 * Same as \ref WorldTransform::map().
 */
inline QPoint operator*(const LonLat& lonLat, const WorldTransform& transform)
{
    return transform.map(lonLat);
}

#endif // WORLDTRANSFORM_H
