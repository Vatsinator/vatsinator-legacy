/*
 * tiledmapdrawer.cpp
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

#include "tiledmapdrawer.h"
#include "tilerenderer.h"
#include <core/geo.h>
#include <core/lonlat.h>
#include <QtGui>

using namespace Vatsinator::Gui;

TiledMapDrawer::TiledMapDrawer(QObject* parent) :
    QObject(parent),
    m_tileRenderer(new TileRenderer) {}

TiledMapDrawer::~TiledMapDrawer()
{
    QThread* t = m_tileRenderer->thread();
    t->quit();
    t->wait();
    
    delete t;
}

void TiledMapDrawer::initialize(MapRenderer* renderer)
{
    m_renderer = renderer;

    QThread* t = new QThread;
    m_tileRenderer->moveToThread(t);
    
    connect(m_tileRenderer, &TileRenderer::mapRendered, this, &TiledMapDrawer::updateMap);
    connect(m_tileRenderer, &TileRenderer::mapRendered, renderer, &MapRenderer::updated);
    connect(renderer, &MapRenderer::viewportChanged, m_tileRenderer, &TileRenderer::updateViewport);
    connect(renderer, &MapRenderer::centerChanged, m_tileRenderer, &TileRenderer::updateCenter);
    connect(renderer, &MapRenderer::zoomChanged, m_tileRenderer, &TileRenderer::updateZoom);
    t->start();
    
    QMetaObject::invokeMethod(m_tileRenderer, "updateViewport", Q_ARG(QSize, renderer->viewport()));
    QMetaObject::invokeMethod(m_tileRenderer, "updateCenter", Q_ARG(Vatsinator::Core::LonLat, renderer->center()));
    QMetaObject::invokeMethod(m_tileRenderer, "updateZoom", Q_ARG(qreal, renderer->zoom()));
}

void TiledMapDrawer::draw(WorldPainter* painter)
{
    painter->drawPixmap(m_coords, m_map);
}

void TiledMapDrawer::updateMap(QRectF coords, QPixmap map)
{
    m_map = map;
    m_coords = coords;
}
