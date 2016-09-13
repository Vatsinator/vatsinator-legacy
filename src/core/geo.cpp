/*
 * geo.cpp
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

#include "geo.h"
#include <QtMath>

namespace Vatsinator { namespace Core {

qreal nmDistance(const LonLat& a, const LonLat& b)
{
    /* http://www.movable-type.co.uk/scripts/latlong.html */
    constexpr qreal R = 3440.06479191; // nm
    
    /* Equirectangular approximation */
    qreal x = (qDegreesToRadians(b.longitude() - a.longitude())) *
        qCos((qDegreesToRadians(a.latitude()) + qDegreesToRadians(b.latitude())) / 2);
    
    qreal y = qDegreesToRadians(b.latitude()) - qDegreesToRadians(a.latitude());
    
    return qSqrt(x * x + y * y) * R;
}

}} /* namespace Vatsinator::Core */
