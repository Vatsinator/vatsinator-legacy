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

namespace TiledMapDrawer {

TiledMapDrawer::TiledMapDrawer(MapRenderer* renderer) :
    m_tileRenderer(new TileRenderer),
    m_renderer(renderer)
{

}

TiledMapDrawer::~TiledMapDrawer()
{

}

void TiledMapDrawer::draw(WorldPainter* painter)
{
    m_tileRenderer->render(painter);
}

} /* namespace TiledMapDrawer */
