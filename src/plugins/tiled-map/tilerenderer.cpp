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
    m_timer(new QTimer(this)),
    m_manager(new TileManager(this))
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &TileRenderer::render);
    connect(m_manager, &TileManager::updated, this, &TileRenderer::render);
}

void TileRenderer::updateViewport(QSize viewport)
{
    m_viewport = viewport;
    m_timer->start(100);
}

void TileRenderer::updateCenter(LonLat center)
{
    m_center = center;
    m_timer->start(100);
}

void TileRenderer::updateZoom(qreal zoom)
{
    m_zoom = zoom;
    m_timer->start(100);
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

void TileRenderer::render()
{
    const qreal dpr = qApp->primaryScreen()->devicePixelRatio();
    WorldTransform transform(m_viewport * dpr, m_center, m_zoom);

    QPixmap px(transform.viewport());
    px.fill(Qt::white);
    WorldPainter p(transform, &px);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    quint32 zoom = zoomLevel(transform);
    QList<Tile> tiles;
    WorldViewport viewport(transform.worldViewport());
    for (auto rect: viewport.rectangles())
        tiles.append(m_manager->tiles(rect, zoom));

    for (auto tile: tiles) {
        QRect source;
        QPixmap px = m_manager->pixmap(tile, &source);
        
        if (!px.isNull())
            p.drawPixmap(tile.coords(), px, source);
    }

    p.end();

    px.setDevicePixelRatio(dpr);
    QRectF screen(transform.map(QPoint(0, 0)), transform.map(QPoint(transform.viewport().width(), transform.viewport().height())));
    emit mapRendered(screen, px);
}
