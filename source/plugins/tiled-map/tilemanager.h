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
#include <QPair>
#include <QList>
#include <QMap>
#include "vatsimdata/lonlat.h"

class FileDownloader;
class MapRenderer;
class Tile;
class TileUrl;
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
    
    QList<Tile*>& tilesForCurrentZoom();
    
    QPair<quint64, quint64> tileForLonLat(const LonLat& lonLat);
    
    /**
     * x and y coordinates of tile at the given coordinate, at the given zoom.
     */
    static QPair<quint64, quint64> tileForLonLat(const LonLat& lonLat, unsigned zoom);
    
private slots:
    void __calculateTileZoom();
    void __updateTileList();
    void __tileDownloaded(const QString& fileName, const QUrl& url);
    
private:
    MapRenderer* __renderer;
    quint32 __tileZoom;
    FileDownloader* __downloader;
    
    typedef QList<Tile*> TileList;
    QMap<quint64, TileList> __tiles; /**< zoom <-> tile list pairs */
    
};

#endif // TILEMANAGER_H
