/*
 * tilemanager.cpp
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

#include <QtCore>
#include <cmath>

#include "network/filedownloader.h"
#include "storage/filemanager.h"
#include "ui/map/maprenderer.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "tile.h"
#include "tilereadyevent.h"

#include "tilemanager.h"

Q_CONSTEXPR quint32 TileWidth = /* in pixels */
#ifdef Q_OS_ANDROID
    512;
#else
    256;
#endif
    
Q_CONSTEXPR int Downloaders = 4; /* number of tile downloaders */

TileManager::TileManager(MapRenderer* renderer, QObject* parent) :
    QObject(parent),
    __renderer(renderer),
    __tileZoom(1)
{
    Q_ASSERT(renderer);
    connect(renderer, &MapRenderer::zoomChanged, this, &TileManager::__calculateTileZoom);
    connect(renderer, &MapRenderer::viewportChanged, this, &TileManager::__calculateTileZoom);
}

void
TileManager::fetchTile(const TileUrl& url)
{
    Q_ASSERT(url.zoom() > 0);
    
//     Was used back then when TileManager existed on its own thread
//     QMetaObject::invokeMethod(this, "__fetchTileImpl", Q_ARG(TileUrl, url));
    
    __fetchTileImpl(url);
}

void
TileManager::initialize()
{
    for (int i = 0; i < Downloaders; ++i) {
        FileDownloader* d = new FileDownloader(this);
        connect(d, &FileDownloader::finished, this, &TileManager::__tileDownloaded);
        __downloaders << d;
    }
    
    /* Fetch the whole 1 zoom */
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            Tile* tile = new Tile(i, j, 1, this);
            connect(tile, &Tile::ready, __renderer, &MapRenderer::updated);
            __tiles[1].insert(std::make_pair(TileCoord(i, j), tile));
        }
    }
}

QList<Tile*>
TileManager::tiles(const QRectF& rect, quint32 zoom)
{
    QRectF realRect = rect.isNull() ? __renderer->screen() : rect;
    int realZoom = zoom ? zoom : __tileZoom;
    
    auto topLeft = tileCoordsForLonLat(realRect.topLeft());
    auto bottomRight = tileCoordsForLonLat(realRect.bottomRight());
    
    if (bottomRight.first < topLeft.first)
        bottomRight = tileCoordsForLonLat(LonLat(180.0, realRect.bottomRight().y()));
    
    QList<Tile*> tiles;
    
    for (quint64 x = topLeft.first; x <= bottomRight.first; ++x)
        for (quint64 y = topLeft.second; y <= bottomRight.second; ++y)
            tiles << this->tile(x, y, realZoom);
    
    return tiles;
}

QPair<quint64, quint64>
TileManager::tileCoordsForLonLat(const LonLat& lonLat)
{
    return tileCoordsForLonLat(lonLat, __tileZoom);
}

Tile*
TileManager::tile(quint64 x, quint64 y, quint64 z)
{
    Q_ASSERT(z > 0);
    TileMap& tiles = __tiles[z];
    auto it = tiles.find(TileCoord(x, y));
    if (it == tiles.end()) {
        Tile* tile = new Tile(x, y, z, this);
        tile->moveToThread(this->thread());
        connect(tile, &Tile::ready, __renderer, &MapRenderer::updated);
        
        tiles.insert(std::make_pair(TileCoord(x, y), tile));
        return tile;
    } else {
        return it->second;
    }
}

QRectF
TileManager::tileCoords(quint64 x, quint64 y, quint64 z)
{
#ifndef qSinh
# define qSinh(x) (0.5 * (qExp(x) - qExp(-x)))
#endif
    
    qreal n = qPow(2.0, static_cast<qreal>(z));
    
    QPointF topLeft;
    topLeft.setX(static_cast<qreal>(x) / n * 360.0 - 180.0);
    topLeft.setY(qRadiansToDegrees(qAtan(qSinh(M_PI * (1 - 2 * static_cast<qreal>(y) / n)))));
    
    QPointF bottomRight;
    bottomRight.setX(static_cast<qreal>(x + 1) / n * 360.0 - 180.0);
    bottomRight.setY(qRadiansToDegrees(qAtan(qSinh(M_PI * (1 - 2 * static_cast<qreal>(y + 1) / n)))));
    
    return QRectF(topLeft, bottomRight);
}

QPair<quint64, quint64>
TileManager::tileCoordsForLonLat(const LonLat& lonLat, unsigned zoom)
{
    LonLat ll = lonLat.bound();
    qreal n = qPow(2.0, static_cast<qreal>(zoom));
    quint64 x = qFloor((ll.longitude() + 180.0) / 360.0 * n);
    qreal lat_rad = qDegreesToRadians(ll.latitude());
    quint64 y = qFloor((1.0 - qLn(qTan(lat_rad) + (1.0 / qCos(lat_rad))) / M_PI) / 2.0 * n);
    
    return qMakePair(x, y);
}

TileUrl
TileManager::__dequeueByPriority()
{
    auto centerCoords = tileCoordsForLonLat(__renderer->center());
    
    auto it = std::min_element(__tileQueue.begin(), __tileQueue.end(), [this, &centerCoords](const TileUrl& a, const TileUrl& b) {
        if (a.zoom() == b.zoom()) { // zooms equal, prioritize by distance from the center of the screen
            return qSqrt(qPow(static_cast<int>(a.x() - centerCoords.first), 2) + qPow(static_cast<int>(a.y() - centerCoords.second), 2)) <
                qSqrt(qPow(static_cast<int>(b.x() - centerCoords.first), 2) + qPow(static_cast<int>(b.y() - centerCoords.second), 2));
        } else {
            return qAbs(__tileZoom - a.zoom()) < qAbs(__tileZoom - b.zoom());
        }
    });
    
    Q_ASSERT(it != __tileQueue.end());
    
    TileUrl tmp = *it;
    __tileQueue.erase(it);
    return tmp;
}

void
TileManager::__fetchTileImpl(const TileUrl& url)
{
    auto it = std::min_element(__downloaders.begin(), __downloaders.end(), [this](FileDownloader* a, FileDownloader* b)  {
        return a->tasks() < b->tasks();
    });
    
    if ((*it)->tasks() > 0) {
        __tileQueue << url;
    } else {
        (*it)->setProperty("tileUrl", QVariant::fromValue<TileUrl>(url));
        (*it)->fetch(url.toUrl());
    }
}

void
TileManager::__calculateTileZoom()
{
    LonLat p1 = __renderer->mapToLonLat(QPoint(0, 0));
    LonLat p2 = __renderer->mapToLonLat(QPoint(TileWidth, 0));
    
    qreal dist = VatsimDataHandler::nmDistance(p2, p1);
    qreal nmPerPix = dist / TileWidth;
    
    /* K = 156543.03 for meters. This is for NM. */
    constexpr qreal K = 84.5264761719;
    
    int zoom = qCeil(std::log2(K * (qCos(qDegreesToRadians(p1.latitude())) / nmPerPix)));
    __tileZoom = qBound(1, zoom, 18);
}

void
TileManager::__tileDownloaded(const QString& fileName, const QUrl& url)
{
    FileDownloader* downloader = qobject_cast<FileDownloader*>(sender());
    Q_ASSERT(downloader);
    Q_ASSERT(downloader->property("tileUrl").isValid());
    Q_ASSERT(downloader->property("tileUrl").canConvert<TileUrl>());
    
    TileUrl tileUrl = downloader->property("tileUrl").value<TileUrl>();
    if (!tileUrl.isValid())
        return;
    
    FileManager::moveToCache(fileName, tileUrl.path());
    
    TileMap& tiles = __tiles[tileUrl.zoom()];
    auto it = std::find_if(tiles.begin(), tiles.end(), [&tileUrl](const std::pair<const TileCoord&, const Tile*>& it) {
        return tileUrl == it.second->url();
    });
    
    Q_ASSERT(it != tiles.end());
    
    TileReadyEvent e;
    qApp->sendEvent(it->second, &e);
    
    Q_ASSERT(downloader->tasks() <= 1); /* One downloader - one task at a time */
    if (__tileQueue.length() > 0) {
        TileUrl url = __dequeueByPriority();
        downloader->setProperty("tileUrl", QVariant::fromValue<TileUrl>(url));
        downloader->fetch(url.toUrl());
    }
    
    Q_UNUSED(url);
}
