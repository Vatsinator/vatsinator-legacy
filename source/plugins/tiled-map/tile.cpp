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
    if (__coords.isNull()) {
        QPointF topLeft;
        
#define qSinh(x) (0.5 * (qExp(x) - qExp(-x)))
        
        qreal n = qPow(2.0, static_cast<qreal>(zoom()));
        topLeft.setX(static_cast<qreal>(x()) / n * 360.0 - 180.0);
        topLeft.setY(qRadiansToDegrees(qAtan(qSinh(M_PI * (1 - 2 * static_cast<qreal>(y()) / n)))));
        
        QPointF bottomRight;
        bottomRight.setX(static_cast<qreal>(x() + 1) / n * 360.0 - 180.0);
        bottomRight.setY(qRadiansToDegrees(qAtan(qSinh(M_PI * (1 - 2 * static_cast<qreal>(y() + 1) / n)))));
        
        __coords = QRectF(topLeft, bottomRight);
    }
    
#undef qSinh
    
    return __coords;
}

QPixmap
Tile::pixmap(QRect* source) const
{
    Q_ASSERT(source);
    
    QMutexLocker l(&__mutex);
    
    if (__pixmap.isNull()) {
        if (zoom() <= 1) {
            *source = __pixmap.rect();
            return __pixmap;
        }
        
        /* If we don't have the tile yet, get one that is less zoomed */
        quint64 x = this->x() / 2;
        quint64 y = this->y() / 2;
        quint64 z = this->zoom() - 1;
        
        TileManager* manager = qobject_cast<TileManager*>(parent());
        Q_ASSERT(manager);
        
        Tile* tile = manager->tile(z, x, y);
        if (!tile) {
            *source = __pixmap.rect();
            return __pixmap;
        }
        
        QPixmap px = tile->pixmap(source);
        
        source->setWidth(source->width() / 2);
        if (this->x() % 2 == 1)
            source->moveLeft(source->left() + source->width());
        
        source->setHeight(source->height() / 2);
        if (this->y() % 2 == 1)
            source->moveTop(source->top() + source->height());
        
        return px;
    } else {
        *source = __pixmap.rect();
        return __pixmap;
    }
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
    
    if (!__pixmap.isNull())
        emit ready();
}

void
Tile::__fetchTile()
{
    TileManager* manager = qobject_cast<TileManager*>(parent());
    Q_ASSERT(manager);
    
    manager->fetchTile(url());
}
