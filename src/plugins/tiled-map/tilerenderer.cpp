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

TileRenderer::TileRenderer(QObject* parent) :
    QObject(parent),
    m_manager(new TileManager(this))
{
}

QImage TileRenderer::render(const QSize& viewport, const LonLat& center, qreal zoom)
{
    const qreal dpr = qApp->primaryScreen()->devicePixelRatio();
    WorldTransform transform(viewport * dpr, center, zoom);

    QImage image(transform.viewport(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    WorldPainter p(transform, &image);
//    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QList<Tile> tiles;
    quint32 level = zoomLevel(transform);
    WorldViewport worldViewport(transform.worldViewport());
    for (const QRectF& rect: qAsConst(worldViewport.rectangles()))
        tiles.append(m_manager->tiles(rect, level));

    for (const Tile& tile: qAsConst(tiles)) {
        QRect source;
        QImage img = m_manager->tileRendered(tile, &source);

        if (!img.isNull())
            p.drawImage(tile.coords(), img, source);
    }

    p.end();
    image.setDevicePixelRatio(dpr);
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
