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
#include <QtCore>

#include "ui/map/mapconfig.h"

#include "worldtransform.h"


namespace {
    
    inline qreal toMercator(qreal lat)
    {
        return qRadiansToDegrees(qLn(qTan(M_PI / 4 + qDegreesToRadians(lat) / 2)));
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
    int y = (-toMercator(lonLat.latitude()) + toMercator(offset().latitude())) * m * scale() / 360.0 + viewport().height() / 2;
    
    return QPoint(x, y);
}

QPointF
WorldTransform::mapF(const LonLat& lonLat) const
{
    qreal m = qMax(viewport().width(), viewport().height());
    
    qreal x = (lonLat.longitude() - offset().longitude()) * m * scale() / 360.0 + static_cast<qreal>(viewport().width()) / 2.0;
    qreal y = (-toMercator(lonLat.latitude()) + toMercator(offset().latitude())) * m * scale() / 360.0 + static_cast<qreal>(viewport().height()) / 2.0;
    
    return QPointF(x, y);
}

QRect
WorldTransform::map(const QRectF& rect) const
{
    return QRect(map(rect.topLeft()), map(rect.bottomRight()));
}

QVector<QPoint>
WorldTransform::map(const QVector<LonLat>& polygon) const
{
    QVector<QPoint> mapped;
    mapped.reserve(polygon.length());
    std::for_each(polygon.begin(), polygon.end(), [this, &mapped](const LonLat& l) {
        mapped << map(l);
    });
    
    return mapped;
}
