/*
 * firarea.cpp
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

#include "db/firdatabase.h"
#include "vatsimdata/fir.h"

#include "firarea.h"

FirArea::FirArea(const Fir* fir, QObject* parent) :
    MapArea (parent),
    __fir(fir),
    __rect(QPointF(fir->data()->header.externities[0].x, fir->data()->header.externities[1].y),
           QPointF(fir->data()->header.externities[1].x, fir->data()->header.externities[0].y))
{
    const FirRecord* f = fir->data();
    __boundaries.reserve(f->borders.length());
    std::for_each(f->borders.begin(), f->borders.end(), [this](const Point& p) {
       __boundaries << LonLat(p);
    });
}

QRectF
FirArea::boundingRect() const
{
    return __rect;
}

bool
FirArea::isVisible() const
{
    return data()->isStaffed();
}

void
FirArea::draw(QPainter* painter, const WorldTransform& transform) const
{
    QColor firColor = QColor(255, 191, 191);
    
    QBrush brush = painter->brush();
    painter->setBrush(QBrush(firColor, Qt::BDiagPattern));
    
    QPen pen = painter->pen();
    painter->setPen(firColor);
    
    auto region = __boundaries * transform;
    painter->drawPolygon(region.data(), region.length());
    
    painter->setPen(pen);
    painter->setBrush(brush);
    
#ifndef QT_NO_DEBUG
    QRect mapped = __rect * transform;
    painter->drawRect(mapped);
#endif
}
