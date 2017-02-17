/*
 * tilemanager.cpp
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

#include "tilemanager.h"
#include <core/geo.h>
#include <core/lonlat.h>
#include <gui/mapscene.h>
#include <misc/filecache.h>
#include <QtGui>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;
using namespace Vatsinator::Misc;

constexpr auto DownloadersCount = 16; // parallel downloaded tiles

namespace {

QPair<qint32, qint32> tileCoords(const LonLat& lonLat, quint32 zoom)
{
    qreal n = qPow(2.0, static_cast<qreal>(zoom));
    qint32 x = qFloor((lonLat.longitude() + 180.0) / 360.0 * n);
    qreal latRad = qDegreesToRadians(qBound(-90.0, lonLat.latitude(), 90.0));
    qint32 y = qFloor((1.0 - qLn(qTan(latRad) + (1.0 / qCos(latRad))) / M_PI) / 2.0 * n);
    
    return qMakePair(x, y);
}

}

TileManager::TileManager(QObject* parent) :
    QObject(parent)
{
    for (auto i = 0; i < DownloadersCount; ++i) {
        FileDownloader* d = new FileDownloader(this);
        connect(d, &FileDownloader::finished, this, &TileManager::tileFetched);
        connect(d, &FileDownloader::error, this, &TileManager::tileError);
        m_downloaders.append(d);
    }
}

TileManager::~TileManager() {}

QList<Tile> TileManager::tiles(QRectF rect, quint32 zoom)
{
    if (m_lastZoom != zoom)
        m_lastZoom = zoom;
    
    LonLat center(rect.center());
    if (m_lastCenter != center)
        m_lastCenter = center;
    
    auto topLeft = ::tileCoords(rect.topLeft(), zoom);
    auto bottomRight = ::tileCoords(rect.bottomRight(), zoom);

    QList<Tile> tiles;
    for (qint32 y = topLeft.second; y <= bottomRight.second; ++y) {
        for (qint32 x = topLeft.first; x <= bottomRight.first; ++x) {
            if (y >= 0 && y < qPow(2, zoom))
                tiles.append(tile(x, y, zoom));
        }
    }
    
    QMetaObject::invokeMethod(this, "triggerDownloaders", Qt::QueuedConnection);
    return tiles;
}

Tile TileManager::tile(quint32 x, quint32 y, quint32 zoom)
{
    Tile tile(x, y, zoom);
    if (!m_tiles.contains(tile))
        m_tiles.insert(tile);
    
    return tile;
}

QImage TileManager::tileRendered(const Tile& tile)
{
    QRect source;
    QImage img = tileRenderedImpl(tile, &source);

    if (source != img.rect()) {
        QImage tmp = img.copy(source);
        img = tmp.scaled(Tile::tileWidth(), Tile::tileHeight());
    }

    return img;
}

QImage TileManager::tileRenderedImpl(const Tile& tile, QRect* source, int levelsLeft)
{
    Q_ASSERT(source);

    if (tile.isCached() || levelsLeft == 0 || tile.zoom() <= 1) {
        const QImage image = tile.image();
        *source = image.rect();
        return image;
    } else {
        /**
         * Pixmap not yet fetched, find a parent one and return it.
         */
        Tile parentTile = this->tile(tile.x() / 2, tile.y() / 2, tile.zoom() - 1);
        QImage image = this->tileRenderedImpl(parentTile, source, levelsLeft - 1);
        source->setWidth(source->width() / 2);
        if (tile.x() % 2 == 1)
            source->moveLeft(source->left() + source->width());

        source->setHeight(source->height() / 2);
        if (tile.y() % 2 == 1)
            source->moveTop(source->top() + source->height());

        return image;
    }
}

QRectF TileManager::tileCoords(quint64 x, quint64 y, quint64 zoom)
{
#ifndef qSinh
# define qSinh(x) (0.5 * (qExp(x) - qExp(-x)))
#endif
    
    qreal n = qPow(2.0, static_cast<qreal>(zoom));
    
    QPointF topLeft;
    topLeft.setX(static_cast<qreal>(x) / n * 360.0 - 180.0);
    topLeft.setY(qRadiansToDegrees(qAtan(qSinh(M_PI * (1.0 - 2.0 * static_cast<qreal>(y) / n)))));
    
    QPointF bottomRight;
    bottomRight.setX(static_cast<qreal>(x + 1) / n * 360.0 - 180.0);
    bottomRight.setY(qRadiansToDegrees(qAtan(qSinh(M_PI * (1.0 - 2.0 * static_cast<qreal>(y + 1) / n)))));
    
    return QRectF(topLeft, bottomRight);
}

QList<FileDownloader*> TileManager::freeDownloaders()
{
    QList<FileDownloader*> downloaders;
    std::copy_if(m_downloaders.begin(), m_downloaders.end(), std::back_inserter(downloaders), [](const FileDownloader* fd) {
        return !fd->isDownloading();
    });
    
    return downloaders;
}

void TileManager::triggerDownloaders()
{
    auto downloaders = freeDownloaders();
    for (auto fd: downloaders)
        fetchNextTile(fd);
}

void TileManager::fetchNextTile(FileDownloader* downloader)
{
    if (QGuiApplication::applicationState() != Qt::ApplicationActive)
        return;
    
    Q_ASSERT(downloader);
    if (downloader->isDownloading())
        return;
    
    Q_ASSERT(!downloader->isDownloading());
//    Q_ASSERT(!downloader->property("tile").isValid());
    QList<Tile> currentZoom;
    std::copy_if(m_tiles.begin(), m_tiles.end(), std::back_inserter(currentZoom), [this](const Tile& t) {
        return t.zoom() == m_lastZoom && !t.isCached() && !m_downloading.contains(t);
    });
    
    if (currentZoom.isEmpty())
        return;
    
    auto it = std::min_element(currentZoom.begin(), currentZoom.end(), [this](const Tile& a, const Tile& b) {
        qreal distA = nmDistance(a.coords().center(), m_lastCenter);
        qreal distB = nmDistance(b.coords().center(), m_lastCenter);
        return distA < distB;
    });
    
    Tile tile = (*it);
    m_downloading.insert(tile);
    QUrl url = tile.url();
    downloader->setProperty("tile", QVariant::fromValue(tile));
    downloader->fetch(url);
}

void TileManager::tileFetched(QUrl url)
{
    Q_UNUSED(url);
    
    FileDownloader* downloader = qobject_cast<FileDownloader*>(sender());
    Q_ASSERT(downloader);
    QVariant tmp = downloader->property("tile");
    Q_ASSERT(tmp.isValid());
    Q_ASSERT(tmp.canConvert<Tile>());
    Tile tile = tmp.value<Tile>();
    
    FileCache::storeInCache(tile.cachePath(), downloader->data());
    m_downloading.remove(tile);
    downloader->setProperty("tile", QVariant());
    emit updated();
}

void TileManager::tileError(QString error, QUrl url)
{
    qWarning("Error downloading %s (%s)", qPrintable(url.toString()), qPrintable(error));
}
