/*
 * map.cpp
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

#include <QtQuick>

#include "plugins/tiled-map/tiledmapdrawer.h"
#include "storage/filemanager.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/quickuserinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "map.h"

Map::Map(QQuickItem* parent) :
    QQuickPaintedItem(parent),
    __renderer(new MapRenderer(this))
{
    setFlag(QQuickItem::ItemHasContents);
    setOpaquePainting(true);
    
    __renderer->setMapDrawer(new TiledMapDrawer);
    connect(__renderer, &MapRenderer::updated, this, &QQuickItem::update);
}

Map::~Map()
{

}

MapItem*
Map::itemUnderPosition(int x, int y)
{
    MapItem* item = const_cast<MapItem*>(__renderer->scene()->nearest(__renderer->mapToLonLat(QPoint(x, y))));
    Q_ASSERT(item);
    QPoint pos = item->position() * __renderer->transform();
    QRect rect(QPoint(0, 0), qui()->minimumTouchTarget());
    rect.moveCenter(pos);
    if (rect.contains(x, y)) {
        return item;
    } else {
        return nullptr;
    }
}

void
Map::paint(QPainter* painter)
{
    __renderer->paint(painter);
}

void
Map::geometryChanged (const QRectF& newGeometry, const QRectF& oldGeometry)
{
     __renderer->setViewport(QSize(newGeometry.size().width(), newGeometry.size().height()));
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void
Map::updateZoom(qreal factor)
{
    __renderer->setZoom(__renderer->zoom() * factor);
}

void
Map::updatePosition(int x, int y)
{
    LonLat center = __renderer->center();
    QPoint p(x, y);
    center -= __renderer->scaleToLonLat(p);
    center.ry() = qBound(-90.0, center.y(), 90.0);
    
    if (center.x() < -180.0)
        center.rx() += 360.0;
        
    if (center.x() > 180.0)
        center.rx() -= 360.0;
        
    __renderer->setCenter(center);
}
