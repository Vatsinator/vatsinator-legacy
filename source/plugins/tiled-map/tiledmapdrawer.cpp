/*
 * tiledmapdrawer.cpp
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

#include "tile.h"
#include "tilemanager.h"
#include "tilereadyevent.h"

#include "tiledmapdrawer.h"

int TileReady = 0;

TiledMapDrawer::TiledMapDrawer(QObject* parent) :
    QObject (parent),
    __tiles(nullptr)
{
    if (!TileReady)
        TileReady = QEvent::registerEventType();
}

void
TiledMapDrawer::initialize(MapRenderer* renderer)
{
    __tiles = new TileManager(renderer, this);
    __renderer = renderer;
}

void
TiledMapDrawer::draw(QPainter* painter, const QTransform& transform)
{
    Q_ASSERT(__tiles);
    
    auto hints = painter->renderHints();
    painter->setRenderHints(hints | QPainter::SmoothPixmapTransform);
    
    auto& tiles = __tiles->tilesForCurrentZoom();
    for (Tile* t: tiles) {
        if (t->pixmap().isNull())
            continue;
        
        QRectF rect = t->coords();
        rect.setTopLeft(rect.topLeft() * transform);
        rect.setBottomRight(rect.bottomRight() * transform);
        
//         qDebug() << rect;
        
        painter->drawPixmap(rect, t->pixmap(), QRectF(QPointF(0.0, 0.0), t->pixmap().size()));
    }
    
    painter->setRenderHints(hints);
}

