/*
 * tilerenderer.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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
#include "tilerenderer.h"
#include "core/geo.h"
#include "gui/worldpainter.h"
#include "gui/worldtransform.h"
#include <QtGui>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;

namespace {
    
    qreal Log2 = 0.6931471805599453094172321214581765680755001343602552;
    
    qreal log2Impl(int x) { return std::log(x) / Log2; }
    
}

class TileRenderer::TileRenderJob : public QRunnable {
public:
    explicit TileRenderJob(TileManager* manager, QList<Tile> tiles,
                           WorldPainter* painter, QMutex* painterMutex) :
        m_manager(manager),
        m_tiles(tiles),
        m_painter(painter),
        m_painterMutex(painterMutex) {}

    void run() override {
        /* how many tile rows is the job rendering */
        int rows = static_cast<int>(m_tiles.last().y() - m_tiles.first().y()) + 1;

        /* how many tile columns */
        int columns = m_tiles.size() / rows;

        /* the surface to render tiles onto */
        QImage image(Tile::tileWidth() * columns, Tile::tileHeight() * rows, QImage::Format_RGB32);

        /* helps tracking offset */
        quint32 firstY = m_tiles.first().y();
        quint32 firstX = m_tiles.first().x();
//        quint32 maxX = std::max(m_tiles.first(), m_tiles.last(), [](auto a, auto b) { return a.x() < b.x(); }).x();

        for (const Tile& tile: qAsConst(m_tiles)) {
            int yOffset = static_cast<int>(tile.y() - firstY) * Tile::tileHeight();
            int xOffset = static_cast<int>(tile.x() - firstX) * Tile::tileWidth();

            QImage tileImg = m_manager->tileRendered(tile);
            Q_ASSERT(tileImg.height() == Tile::tileHeight());
            Q_ASSERT(tileImg.width() == Tile::tileWidth());
            Q_ASSERT(tileImg.format() == image.format());

            for (int i = 0; i < tileImg.height(); ++i) {
                QRgb* origLine = reinterpret_cast<QRgb*>(tileImg.scanLine(i));
                QRgb* destLine = reinterpret_cast<QRgb*>(image.scanLine(i + yOffset));
                memcpy(destLine + xOffset, origLine, static_cast<size_t>(tileImg.bytesPerLine()));
            }
        }

        QRectF coords = QRectF(m_tiles.first().coords().topLeft(), m_tiles.last().coords().bottomRight());
        m_painterMutex->lock();
        m_painter->drawImage(coords, image);
        m_painterMutex->unlock();
    }

private:
    TileManager* m_manager;
    QList<Tile> m_tiles;
    WorldPainter* m_painter;
    QMutex* m_painterMutex;
};

TileRenderer::TileRenderer(QObject* parent) :
    QObject(parent),
    m_manager(new TileManager(this)),
    m_threadPool(new QThreadPool(this)) {}

void TileRenderer::render(WorldPainter *painter)
{
    QList<Tile> tiles;
    quint32 level = zoomLevel(painter->transform());
    WorldViewport worldViewport(painter->transform().worldViewport());

    for (const QRectF& rect: qAsConst(worldViewport.rectangles()))
        tiles.append(m_manager->tiles(rect, level));

    QList<TileRenderJob*> jobs;
    int n = m_threadPool->maxThreadCount();
    int rows = qAbs(static_cast<int>(tiles.first().y()) - static_cast<int>(tiles.last().y())) + 1;
    quint32 y = tiles.first().y();

    QMutex painterMutex;

    for (int i = 0; i < n; ++i) {
        quint32 rowsToRender = static_cast<quint32>(qCeil(static_cast<qreal>(rows) / (n - i)));
        rows -= rowsToRender;

        if (rowsToRender == 0)
            continue;

        TileRenderJob* job = new TileRenderJob(m_manager, selectRows(tiles, y, y + rowsToRender), painter, &painterMutex);
        jobs.append(job);
        m_threadPool->start(job);

        y += rowsToRender;
    }

    m_threadPool->waitForDone();
}

QList<Tile> TileRenderer::selectRows(const QList<Tile>& tiles, quint32 from, quint32 to)
{
    QList<Tile> tilesPicked;
    for (const Tile& tile: qAsConst(tiles)) {
        if (tile.y() >= from && tile.y() < to)
            tilesPicked.append(tile);
    }

    return tilesPicked;
}

quint32 TileRenderer::zoomLevel(const WorldTransform& transform)
{
    LonLat p1 = transform.map(QPoint(0, 0)).bound();
    LonLat p2 = transform.map(QPoint(256, 0)).bound();
    LonLat p3 = transform.map(QPoint(512, 0)).bound();

    qreal dist1 = nmDistance(p2, p1);
    qreal dist2 = nmDistance(p3, p2);
    qreal dist = qMin(dist1, dist2);
    qreal nmPerPix = dist / 256;

    /* K = 156543.03 for meters. This is for nautical miles. */
    constexpr qreal K = 84.5264761719;

    int zoom = qCeil(log2Impl(K * (qFastCos(qDegreesToRadians(p1.latitude())) / nmPerPix)));
    return qBound(1, zoom, 18);
}
