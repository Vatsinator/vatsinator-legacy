/*
 * tile.cpp
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

#include "tile.h"
#include "tilemanager.h"
#include <misc/filecache.h>
#include <QtGui>

using namespace Vatsinator::Misc;

const QImage NullTile = []() {
    QImage image(QSize(256, 256), QImage::Format_ARGB32_Premultiplied);
    image.fill(QColor(232, 232, 232));
    
    QPainter p(&image);
    QPen pen(QColor(226, 226, 226));
    p.setPen(pen);
    
    for (int i = 8; i < 256; i += 16)
        p.drawLine(i, 0, i, 256);
    
    for (int i = 8; i < 256; i += 16)
        p.drawLine(0, i, 256, i);
    
    return image;
}();

/* Type of tiles: light_nolabels/dark_nolabels */
constexpr auto TileType = "light_nolabels";


/* We cannot use the QPixmapCache here, as the whole purpose of using
 * TileRenderer is to run it on a separate thread.
 */
static QThreadStorage<QCache<QString, QPixmap>> pixmapCache;

static QCache<QString, QImage> tileCache; /**< Stores loaded tiles */


Tile::Tile(quint32 x, quint32 y, quint32 zoom) :
    m_x(x), m_y(y), m_zoom(zoom)
{
    m_cacheKey = QStringLiteral("tiled-map/tiles/%1/%2/%3/%4").arg(
        TileType,
        QString::number(m_zoom),
        QString::number(m_x),
        QString::number(m_y));
}

Tile::Tile(const Tile& other) :
    m_x(other.m_x),
    m_y(other.m_y),
    m_zoom(other.m_zoom),
    m_coords(other.m_coords),
    m_coordsCalculated(other.m_coordsCalculated),
    m_cacheKey(other.m_cacheKey) {}

QRectF Tile::coords() const
{
    if (!m_coordsCalculated) {
        m_coords = TileManager::tileCoords(x(), y(), zoom());
        m_coordsCalculated = true;
    }
    
    return m_coords;
}

QImage Tile::image() const
{
    if (tileCache.contains(cacheKey()))
        return *(tileCache.object(cacheKey()));
    else
        return load();
}

bool Tile::isCached() const
{
    return FileCache::exists(cachePath());
}

QUrl Tile::url() const
{
    static QVector<QString> prefixes = { "a", "b", "c", "d", "e", "f", "g",
                                         "h", "i", "j", "k", "l", "m", "n",
                                         "o", "p", "q", "r", "s", "t", "u",
                                         "v", "w", "x", "y", "z" };
    
    QString prefix = prefixes[qrand() % prefixes.size()];
    return QUrl(QStringLiteral("http://%1.basemaps.cartocdn.com/%2/%3/%4/%5.png")
        .arg(
            prefix,
            TileType,
            QString::number(zoom()),
            QString::number(x()),
            QString::number(y())
        )
                );
}

QString Tile::cachePath() const
{
    return QStringLiteral("tiled-map/tiles/%1/%2/%3/%4.png").arg(
        TileType,
        QString::number(zoom()),
        QString::number(x()),
        QString::number(y()));
}

QImage Tile::load() const
{
    QString path = cachePath();
    
    if (FileCache::exists(path)) {
        QImage* img = new QImage(QImage(FileCache::path(path)).convertToFormat(QImage::Format_ARGB32_Premultiplied));
        if (!img->isNull()) {
            tileCache.insert(cacheKey(), img);
            return *img;
        }
    }
    
    return NullTile;
}
