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

Q_CONSTEXPR quint32 TileWidth = 256; /* in pixels */
Q_CONSTEXPR quint32 TileHeight = 256;

TileManager::TileManager(MapRenderer* renderer, QObject* parent) :
    QObject(parent),
    __renderer(renderer),
    __tileZoom(1),
    __downloader(new FileDownloader(this))
{
    Q_ASSERT(renderer);
    connect(renderer, &MapRenderer::zoomChanged, this, &TileManager::__calculateTileZoom);
    connect(renderer, &MapRenderer::viewportChanged, this, &TileManager::__calculateTileZoom);
    
    connect(renderer, &MapRenderer::zoomChanged, this, &TileManager::__updateTileList);
    connect(renderer, &MapRenderer::viewportChanged, this, &TileManager::__updateTileList);
    connect(renderer, &MapRenderer::centerChanged, this, &TileManager::__updateTileList);
    
    connect(__downloader, &FileDownloader::finished, this, &TileManager::__tileDownloaded);
}

void
TileManager::fetchTile(const TileUrl& url)
{
    if (url.zoom() == 0)
        qFatal("Fetching tile with zoom 0! (%s)", qPrintable(url.toUrl().toString()));
    
    __downloader->fetch(url.toUrl());
}

QList<Tile*>&
TileManager::tilesForCurrentZoom()
{
    TileList& l = __tiles[__tileZoom];
    return l;
}

QPair<quint64, quint64>
TileManager::tileForLonLat (const LonLat& lonLat)
{
    return tileForLonLat(lonLat, __tileZoom);
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
    auto topLeft = tileForLonLat(__renderer->screen().topLeft());
    auto bottomRight = tileForLonLat(__renderer->screen().bottomRight());
    
    TileList& tiles = __tiles[__tileZoom];
    
    for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
        for (auto j = topLeft.second; j <= bottomRight.second; ++j) {
            auto it = std::find_if(tiles.begin(), tiles.end(), [i, j](Tile* tile) {
                return tile->x() == i && tile->y() == j;
            });
            
            if (it == tiles.end()) {
                Tile* tile = new Tile(i, j, __tileZoom, this);
                connect(tile, &Tile::ready, __renderer, &MapRenderer::updated);
                tiles << tile;
            }
        }
    }
    
//     qDebug() << "Tiles updated;" << tiles.size() << "tiles for zoom:" << __tileZoom;
}

void
TileManager::__tileDownloaded(const QString& fileName, const QUrl& url)
{
    TileUrl tileUrl = TileUrl::fromUrl(url);
    if (!tileUrl.isValid())
        return;
    
    FileManager::moveToCache(fileName, tileUrl.path());
    
    TileList& tiles = __tiles[tileUrl.zoom()];
    auto it = std::find_if(tiles.begin(), tiles.end(), [&tileUrl](Tile* t) {
        return tileUrl == t->url();
    });
    
    Q_ASSERT(it != tiles.end());
    
    TileReadyEvent e;
    qApp->notify(*it, &e);
}
