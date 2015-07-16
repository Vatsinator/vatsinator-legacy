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
    __downloaded(false),
    __url(x, y, zoom),
    __tileTexture(nullptr)
{
    QString path = __url.path();
    CacheFile cached(path);
    if (!cached.exists() || QImage(QFileInfo(cached).absoluteFilePath()).isNull()) {
        TileManager* manager = qobject_cast<TileManager*>(parent);
        Q_ASSERT(manager);
        
        manager->fetchTile(url());
    } else {
        __downloaded = true;
    }
}

Tile::~Tile()
{
    if (__tileTexture)
        delete __tileTexture;
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
    
    return __coords;
}

QOpenGLTexture*
Tile::texture()
{
    if (!__tileTexture) {
        CacheFile cached(__url.path());
        QImage image(QFileInfo(cached).absoluteFilePath());
        
        if (image.isNull())
            return nullptr;
        
        __tileTexture = new QOpenGLTexture(image.mirrored(), QOpenGLTexture::DontGenerateMipMaps);
    }
    
    return __tileTexture;
}

void Tile::customEvent(QEvent* event)
{
    if (event->type() == TileReady) {
        __downloaded = true;        
    }
}

void
Tile::__loadTile()
{
    
}
