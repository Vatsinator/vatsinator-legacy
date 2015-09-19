/*
 * tmaarea.cpp
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

#include <QtGui>

#include "ui/map/mapscene.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/tma.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "tmaarea.h"

TmaArea::TmaArea(const Airport* airport, QObject* parent) :
    MapArea (parent),
    __scene(qobject_cast<MapScene*>(parent))
{
    Q_ASSERT(__scene);
    
    __initialize(airport);
}

QRectF
TmaArea::boundingRect() const
{
    return __rect;
}

bool
TmaArea::isVisible() const
{
    return true;
}

void
TmaArea::draw(QPainter* painter, const WorldTransform& transform) const
{
    QBrush brush = painter->brush();
    painter->setBrush(QColor(62, 87, 127, 50));
    
    QPen pen = painter->pen();
    painter->setPen(QColor(62, 87, 127, 50));
    
    if (__boundaries.isEmpty()) {
        QRect rect = __rect * transform;
        painter->drawEllipse(rect);
    } else {
        auto region = __boundaries * transform;
        painter->drawPolygon(region.data(), region.length());
    }
    
    painter->setPen(pen);
    painter->setBrush(brush);
}

void
TmaArea::__initialize(const Airport* airport)
{
    Tma* tma = vApp()->vatsimDataHandler()->findTma(airport->icao());
    if (tma) {
        for (const Point& p: tma->points()) {
            __boundaries << p;
        }
        __boundingRectFromBoundaries();
    } else {
        __rect = QRectF(QPointF(0.0, 0.0), QSizeF(4.0, -3.0));
        __rect.moveCenter(airport->position());
    }
}

void
TmaArea::__boundingRectFromBoundaries()
{
    auto xMinMax = std::minmax_element(__boundaries.begin(), __boundaries.end(), [](const LonLat& a, const LonLat& b) {
        return a.x() < b.x();
    });
    
    qreal minX = xMinMax.first->x();
    qreal maxX = xMinMax.second->x();
    
    auto yMinMax = std::minmax_element(__boundaries.begin(), __boundaries.end(), [](const LonLat& a, const LonLat& b) {
        return a.y() < b.y();
    });
    
    qreal minY = yMinMax.first->y();
    qreal maxY = yMinMax.second->y();
    
    __rect = QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}
