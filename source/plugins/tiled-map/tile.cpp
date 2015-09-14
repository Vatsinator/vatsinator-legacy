/*
 * tile.cpp
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

#include "network/filedownloader.h"
#include "storage/cachefile.h"

#include "tilemanager.h"
#include "tilereadyevent.h"

#include "tile.h"

Tile::Tile(quint64 x, quint64 y, quint64 zoom, QObject* parent) :
    QObject(parent),
    __ready(false),
    __x(x),
    __y(y),
    __zoom(zoom),
    __url(x, y, zoom)
{
    QString path = __url.path();
    CacheFile cached(path);
    if (!cached.exists()) {
        __fetchTile();
    } else {
        __loadTile();
    }
}

const QRectF&
Tile::coords() const
{
    if (__coords.isNull())
        __coords = TileManager::tileCoords(x(), y(), zoom());
    
    return __coords;
}

QPixmap
Tile::pixmap() const
{
    return __pixmap;
}

void Tile::customEvent(QEvent* event)
{
    if (event->type() == TileReady) {
        __loadTile();
    }
}

void
Tile::__loadTile()
{
    CacheFile cached(__url.path());
    
    QMutexLocker l(&__mutex);
    __pixmap.load(QFileInfo(cached).absoluteFilePath());
    
    if (!__pixmap.isNull()) {
        __ready = true;
        emit ready();
    }
}

void
Tile::__fetchTile()
{
    TileManager* manager = qobject_cast<TileManager*>(parent());
    Q_ASSERT(manager);
    
    manager->fetchTile(url());
}
