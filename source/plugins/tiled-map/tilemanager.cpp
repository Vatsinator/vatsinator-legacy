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
    
//    connect(renderer, &MapRenderer::zoomChanged, this, &TileManager::__updateTileList);
//    connect(renderer, &MapRenderer::viewportChanged, this, &TileManager::__updateTileList);
//    connect(renderer, &MapRenderer::centerChanged, this, &TileManager::__updateTileList);
}

void
TileManager::fetchTile(const TileUrl& url)
{
    Q_ASSERT(url.zoom() > 0);
    QMetaObject::invokeMethod(this, "__fetchTileImpl", Q_ARG(TileUrl, url));
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
TileManager::tilesForCurrentZoom()
{   
    auto topLeft = tileCoordForLonLat(__renderer->screen().topLeft());
    auto bottomRight = tileCoordForLonLat(__renderer->screen().bottomRight());
    
    if (bottomRight.first < topLeft.first)
        bottomRight = tileCoordForLonLat(LonLat(180.0, __renderer->screen().bottomRight().y()));
    
    __tileMutex.lock();
    
    TileMap& tiles = __tiles[__tileZoom];
    QList<Tile*> l;
    
    for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
        for (auto j = topLeft.second; j <= bottomRight.second; ++j) {
            auto it = tiles.find(TileCoord(i, j));
            
            if (it != tiles.end()) {
                l << it->second;
            }
        }
    }
    
    __tileMutex.unlock();
    return l;
}

void
TileManager::forEachTileInRect(const LonLat& topLeft, const LonLat& bottomRight,
                               std::function<void(const Tile*)> function)
{
    auto tileTopLeft = tileCoordForLonLat(topLeft);
    auto tileBottomRight = tileCoordForLonLat(bottomRight);
    
    if (tileBottomRight.first < tileTopLeft.first)
        tileBottomRight = tileCoordForLonLat(LonLat(180.0, bottomRight.y()));
    
    __tileMutex.lock();
    
    for (auto i = tileTopLeft.first; i <= tileBottomRight.first; ++i) {
        for (auto j = tileTopLeft.second; j <= tileBottomRight.second; ++j) {
            Tile* tile = this->tile(i, j, __tileZoom);
            Q_ASSERT(tile);
            function(tile);
        }
    }
    
    __tileMutex.unlock();
}

void
TileManager::forEachTileOnScreen(std::function<void(const Tile*)> function)
{
    forEachTileInRect(__renderer->screen().topLeft(), __renderer->screen().bottomRight(), function);
}

QPair<quint64, quint64>
TileManager::tileCoordForLonLat(const LonLat& lonLat)
{
    return tileForLonLat(lonLat, __tileZoom);
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
TileManager::tileForLonLat(const LonLat& lonLat, unsigned zoom)
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
    auto it = std::min_element(__tileQueue.begin(), __tileQueue.end(), [this](const TileUrl& a, const TileUrl& b) {
        return qAbs(__tileZoom - a.zoom()) < qAbs(__tileZoom - b.zoom());
    });
    
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
    
    int zoom = 0;
    qreal resolution;
    do {
        zoom += 1;
        resolution = 84.5264761719 * qCos(qDegreesToRadians(p1.latitude())) / (qPow(2.0, zoom));
    } while (resolution > nmPerPix);
    
    __tileZoom = zoom;
}

void
TileManager::__updateTileList()
{
    auto topLeft = tileCoordForLonLat(__renderer->screen().topLeft());
    auto bottomRight = tileCoordForLonLat(__renderer->screen().bottomRight());
    
    if (bottomRight.first < topLeft.first)
        bottomRight = tileCoordForLonLat(LonLat(180.0, __renderer->screen().bottomRight().y()));
    
    __tileMutex.lock();
    TileMap& tiles = __tiles[__tileZoom];
    __tileMutex.unlock();
    
    for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
        for (auto j = topLeft.second; j <= bottomRight.second; ++j) {
            TileCoord coord(i, j);
            
            auto it = tiles.find(coord);
            
            if (it == tiles.end()) {
                Tile* tile = new Tile(i, j, __tileZoom, this);
                connect(tile, &Tile::ready, __renderer, &MapRenderer::updated);
                
                __tileMutex.lock();
                tiles.insert(std::make_pair(coord, tile));
                __tileMutex.unlock();
            }
        }
    }
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
    __queueMutex.lock();
    if (__tileQueue.length() > 0) {
        TileUrl url = __dequeueByPriority();
        downloader->setProperty("tileUrl", QVariant::fromValue<TileUrl>(url));
        downloader->fetch(url.toUrl());
    }
    __queueMutex.unlock();
    
    Q_UNUSED(url);
}
