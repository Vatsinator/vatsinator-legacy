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

TiledMapDrawer::~TiledMapDrawer() {}

void TiledMapDrawer::initialize(MapRenderer* renderer)
{
    m_renderer = renderer;
}

void TiledMapDrawer::draw(WorldPainter* painter)
{
    QImage world = m_tileRenderer->render(m_renderer->viewport(), m_renderer->center(), m_renderer->zoom());
    painter->drawImage(QPoint(0, 0), world);
}
