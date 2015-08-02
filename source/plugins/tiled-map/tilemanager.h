/*
 * tilemanager.h
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

#ifndef TILEMANAGER_H
#define TILEMANAGER_H

#include <QObject>
#include <QMutex>
#include <QPair>
#include <QList>
#include <QMap>
#include <spatial/bits/spatial_check_concept.hpp>
#include <spatial/box_multimap.hpp>
#include "vatsimdata/lonlat.h"
#include "tileurl.h"

class FileDownloader;
class MapRenderer;
class Tile;
class QUrl;

/**
 * http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
 */
class TileManager : public QObject {
    Q_OBJECT

public:
    explicit TileManager(MapRenderer* renderer, QObject *parent = nullptr);
    
    /**
     * Downloads a tile.
     */
    void fetchTile(const TileUrl& url);
    
    QList<Tile*> tilesForCurrentZoom();
    
    QPair<quint64, quint64> tileCoordForLonLat(const LonLat& lonLat);
    
    Tile* tile(quint64 z, quint64 x, quint64 y);
    
    /**
     * x and y coordinates of tile at the given coordinate, at the given zoom.
     */
    static QPair<quint64, quint64> tileCoordForLonLat(const LonLat& lonLat, unsigned zoom);
    
public slots:
    /**
     * Must be called on its own thread.
     */
    void initialize();
    
private slots:
    void __calculateTileZoom();
    void __updateTileList();
    void __tileDownloaded(const QString& fileName, const QUrl& url);
    
private:
    MapRenderer* __renderer;
    quint32 __tileZoom;
    QList<FileDownloader*> __downloaders;
    QList<TileUrl> __tileQueue;
    QMutex __mutex;
    
    typedef QPair<quint64, quint64> TileCoord;
    
    struct TileCoordAccessor
    {
        quint64 operator() (spatial::dimension_type dim, const TileCoord& coord) const
        {
            switch (dim) {
                case 0: return coord.first;
                case 1: return coord.second;
                default: Q_UNREACHABLE();
            }
        }
    };
    
    typedef spatial::box_multimap<2, TileCoord, Tile*, spatial::accessor_less<TileCoordAccessor, TileCoord>> TileMap;
    QMap<quint64, TileMap> __tiles; /**< zoom <-> tile map pairs */
    
};

#endif // TILEMANAGER_H
