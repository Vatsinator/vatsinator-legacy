/*
 * tile.h
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

#ifndef TILE_H
#define TILE_H

#include <QPixmap>
#include <QRectF>
#include <QUrl>
#include <QString>

class TileManager;

/**
 * \ingroup Tiled-mad-drawer
 * @{
 */
class Tile {
    
    friend class TileManager;
    
public:
    Tile() = default;
    Tile(quint32 x, quint32 y, quint32 zoom);
    Tile(const Tile& other);
    
    /**
     * Gets the tile's rectangle global coords.
     */
    QRectF coords() const;

    /**
     * Gets the tile's image.
     */
    QImage image() const;
    
    /**
     * Returns \c true if the tile is already downloaded.
     */
    bool isCached() const;
    
    /**
     * URL where the Tile is available at.
     */
    QUrl url() const;
    
    quint32 x() const { return m_x; }
    quint32 y() const { return m_y; }
    quint32 zoom() const { return m_zoom; }
    
    const QString& cacheKey() const { return m_cacheKey; }
    
private:
    QString cachePath() const;
    QImage load() const;
    
    quint32 m_x = 0;
    quint32 m_y = 0;
    quint32 m_zoom = 0;
    
    mutable QRectF m_coords;
    mutable bool m_coordsCalculated = false;
    
    QString m_cacheKey;
    
}; /** @} */

inline bool operator==(const Tile& a, const Tile& b)
{
    return a.zoom() == b.zoom() && a.x() == b.x() && a.y() == b.y();
}

inline uint qHash(const Tile& key, uint seed)
{
    return qHash(key.x(), seed) ^ qHash(key.y(), seed) ^ qHash(key.zoom(), seed);
}

Q_DECLARE_METATYPE(Tile)

#endif // TILE_H
