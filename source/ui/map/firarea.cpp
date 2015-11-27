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
#include "ui/map/firitem.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "vatsimdata/fir.h"

#include "firarea.h"

FirArea::FirArea(const Fir* fir, const FirRecord* record, const FirItem* item, QObject* parent) :
    MapArea(parent),
    __scene(qobject_cast<MapScene*>(parent)),
    __fir(fir),
    __rect(QPointF(record->header.externities[0].x, record->header.externities[1].y),
           QPointF(record->header.externities[1].x, record->header.externities[0].y)),
    __item(item),
    __icao(QString::fromUtf8(record->header.icao))
{
    __borders.reserve(record->borders.length() * 2);
    __polygon.reserve(record->borders.length());
    std::for_each(record->borders.begin(), record->borders.end(), [this](const Point& p) {
        if (__borders.length() > 1)
            __borders << __borders.last();
        
       __borders << LonLat(p);
       __polygon << LonLat(p);
    });
    
    __borders << __borders.last() << __borders.first();
    
    for (auto it = __borders.begin(); it < __borders.end(); it += 2) {
        if (qAbs(it->x()) == 180.0 && qAbs((it + 1)->x()) == 180.0) {
            it = __borders.erase(it, it + 2);
        }
    }
}

QRectF
FirArea::boundingRect() const
{
    return __rect;
}

bool
FirArea::isVisible() const
{
    if (__scene->renderer()->isRendering())
        return data()->isStaffed() || __scene->renderer()->selectedItems().contains((MapItem*&)__item);
    else
        return data()->isStaffed();
}

void
FirArea::draw(QPainter* painter, const WorldTransform& transform) const
{
    QColor firColor = QColor(175, 175, 175);
    if (data()->isStaffed())
        firColor = QColor(176, 32, 32, 30);

    QPen pen = painter->pen();
    painter->setPen(firColor);
    
    QPainter::RenderHints hints = painter->renderHints();
    painter->setRenderHints(hints | QPainter::Antialiasing);
    
    auto lines = __borders * transform;
    painter->drawLines(lines);
    
    if (data()->isStaffed()) {
        QBrush brush = painter->brush();
        painter->setBrush(QBrush(firColor));
        
        auto polygon = __polygon * transform;
        painter->drawPolygon(polygon.constData(), polygon.size());
        
        painter->setBrush(brush);
    }
    
    painter->setPen(pen);
    painter->setRenderHints(hints);
    
#ifndef QT_NO_DEBUG
    QRect mapped = __rect * transform;
    painter->drawRect(mapped);
    
    painter->drawText(__polygon.first() * transform, __icao);
#endif
}
