/*
 * worldtransform.cpp
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

#include <QtMath>

#include "ui/map/mapconfig.h"

#include "worldtransform.h"


namespace {
    
    inline qreal toMercator(qreal lat)
    {
        return qRadiansToDegrees(qLn(qTan(M_PI / 4 + qDegreesToRadians(lat) / 2)));
    }
    
    inline qreal fromMercator(qreal y)
    {
        return qRadiansToDegrees(2 * qAtan(qExp(qDegreesToRadians(y))) - M_PI / 2);
    }
    
}


WorldTransform::WorldTransform(const QSize& viewport, const LonLat& offset, qreal scale) :
    __viewport(viewport),
    __offset(offset),
    __scale(scale)
{

}

QPoint
WorldTransform::map(const LonLat& lonLat) const
{    
    qreal m = qMax(viewport().width(), viewport().height());
    
    int x = (lonLat.longitude() - offset().longitude()) * m * scale() / 360.0 + viewport().width() / 2;
    int y = (-toMercator(lonLat.latitude()) + offset().latitude()) * m * scale() / 360.0 + viewport().height() / 2;
    
    return QPoint(x, y);
}