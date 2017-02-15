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
    explicit TileRenderJob(TileManager* manager, const QList<Tile>& tiles, QImage* image) :
        m_manager(manager), m_tiles(tiles), m_image(image) {}

    void run() override {
        int xOffset = 0;
        for (const Tile& tile: qAsConst(m_tiles)) {
            QRect source;
            QImage img = m_manager->tileRendered(tile, &source);
            Q_ASSERT(img.format() == m_image->format());

            if (!img.isNull()) {
                int height = img.height();
                for (int i = 0; i < height; ++i) {
                    QRgb* origLine = reinterpret_cast<QRgb*>(img.scanLine(i));
                    QRgb* destLine = reinterpret_cast<QRgb*>(m_image->scanLine(i));
                    memcpy(destLine + xOffset, origLine, img.bytesPerLine());
                }

                xOffset += img.bytesPerLine();
            }
        }
    }

private:
    TileManager* m_manager;
    const QList<Tile>& m_tiles;
    QImage* m_image;

};

TileRenderer::TileRenderer(QObject* parent) :
    QObject(parent),
    m_manager(new TileManager(this)),
    m_threadPool(new QThreadPool(this))
{
}

QImage TileRenderer::render(const QSize& viewport, const LonLat& center, qreal zoom)
{
    const qreal dpr = qApp->primaryScreen()->devicePixelRatio();
    WorldTransform transform(viewport * dpr, center, zoom);

    QImage image(transform.viewport(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

//    WorldPainter p(transform, &image);
//    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QList<Tile> tiles;
    quint32 level = zoomLevel(transform);
    WorldViewport worldViewport(transform.worldViewport());

    QElapsedTimer timer;
    timer.start();

    for (const QRectF& rect: qAsConst(worldViewport.rectangles()))
        tiles.append(m_manager->tiles(rect, level));

    QList<Tile> tilesPicked;
    quint32 y = tiles.first().y() + 1;
    for (const Tile& tile: qAsConst(tiles)) {
        if (tile.y() == y)
            tilesPicked.append(tile);
    }

    qDebug() << "Picking tiles:" << timer.elapsed() << "ms";
    timer.restart();

    TileRenderJob* job = new TileRenderJob(m_manager, tilesPicked, &image);
    m_threadPool->start(job);

    m_threadPool->waitForDone();

//    p.end();
    image.setDevicePixelRatio(dpr);

    qDebug() << "Drawing tiles:" << timer.elapsed() << "ms";

    return image;
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
