/*
 * tilemanager.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#include "tile.h"
#include <core/filedownloader.h>
#include <core/lonlat.h>
#include <QHash>
#include <QObject>
#include <QRectF>
#include <QPixmap>
#include <QScopedPointer>
#include <QSet>

struct TileCoords;

/**
 * \ingroup Tiled-mad-drawer
 * @{
 */
class TileManager : public QObject {
    Q_OBJECT
    
signals:
    /**
     * An update request.
     */
    void updated();

public:
    explicit TileManager(QObject* parent = nullptr);
    virtual ~TileManager();
    
    /**
     * Returns list of tiles that are inside the given rectangle,
     * for the given zoom.
     */
    QList<Tile> tiles(QRectF rect, quint32 zoom);
    
    /**
     * Returns the tile under the provided coordinates.
     */
    Tile tile(quint32 x, quint32 y, quint32 zoom);

    /**
     * Gets the rendered image of the given tile.
     */
    QImage tileRendered(const Tile& tile);
    
    /**
     * Calculate global coordinates for the given tile.
     */
    static QRectF tileCoords(quint64 x, quint64 y, quint64 zoom);
    
private:
    QImage tileRenderedImpl(const Tile& tile, QRect* source, int levelsLeft = 5);
    QList<Vatsinator::Core::FileDownloader*> freeDownloaders();
    
private slots:
    void triggerDownloaders();
    void fetchNextTile(Vatsinator::Core::FileDownloader* downloader);
    void tileFetched(QUrl url);
    void tileError(QString error, QUrl url);
    
private:
    QList<Vatsinator::Core::FileDownloader*> m_downloaders;
    quint32 m_lastZoom = 0;
    Vatsinator::Core::LonLat m_lastCenter;
    QSet<Tile> m_tiles;
    QSet<Tile> m_downloading;
    
}; /** @} */

#endif // TILEMANAGER_H
