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

#include "ui/map/maprenderer.h"

#include "tile.h"
#include "tilemanager.h"
#include "tilereadyevent.h"

#include "tiledmapdrawer.h"

int TileReady = 0; /* Event */

TiledMapDrawer::TiledMapDrawer(QObject* parent) :
    QObject (parent),
    __tiles(nullptr)
{
    if (!TileReady)
        TileReady = QEvent::registerEventType();
}

TiledMapDrawer::~TiledMapDrawer()
{
    QThread* thread = __tiles->thread();
    __tiles->deleteLater();
    thread->quit();
    thread->wait();
}

void
TiledMapDrawer::initialize(MapRenderer* renderer)
{
    __tiles = new TileManager(renderer);
    QThread* thread = new QThread(this);
    __tiles->moveToThread(thread);
    connect(thread, &QThread::started, __tiles, &TileManager::initialize);
    thread->start();
    
    __renderer = renderer;
}

void
TiledMapDrawer::draw(QPainter* painter, const WorldTransform& transform)
{
    Q_ASSERT(__tiles);
    
    auto hints = painter->renderHints();
    painter->setRenderHints(hints | QPainter::SmoothPixmapTransform);
    
    auto tiles = __tiles->tilesForCurrentZoom();
    for (Tile* t: tiles) {
        QRect rect(t->coords().topLeft() * transform, t->coords().bottomRight() * transform);
        
        if (!t->pixmap().isNull())
            painter->drawPixmap(rect, t->pixmap());
        
#ifndef QT_NO_DEBUG
        /* Extra info */
        painter->drawText(rect, Qt::AlignTop | Qt::AlignLeft, QStringLiteral("%1/%2/%3").arg(
            QString::number(t->zoom()),
            QString::number(t->x()),
            QString::number(t->y())));
        painter->drawRect(rect);
#endif
    }
    
    painter->setRenderHints(hints);
}

