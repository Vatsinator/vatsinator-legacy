/*
 * tiledmapdrawer.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include <QtGui>

#include "tile.h"
#include "tilemanager.h"
#include "tilereadyevent.h"

#include "tiledmapdrawer.h"

int TileReady = 0;

TiledMapDrawer::TiledMapDrawer(QObject* parent) :
    QObject (parent),
    __tiles(nullptr)
{
    if (!TileReady)
        TileReady = QEvent::registerEventType();
}

MapDrawer::MapDrawerFlags
TiledMapDrawer::flags() const
{
    return 0;
}

void
TiledMapDrawer::initialize(MapRenderer* renderer)
{
    __tiles = new TileManager(renderer, this);
    __renderer = renderer;
}

void
TiledMapDrawer::draw(const QMatrix4x4& mvp, const QRectF& screen, const QSize& viewport)
{
    Q_ASSERT(__tiles);
    
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
    
    gl->glClearColor(1.0, 1.0, 1.0, 1.0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    auto& tiles = __tiles->tilesForCurrentZoom();
    for (Tile* t: tiles) {
        if (!t->texture())
            continue;
        
        LonLat topLeft = __renderer->glFromLonLat(t->coords().topLeft());
        LonLat bottomRight = __renderer->glFromLonLat(t->coords().bottomRight());
        
        t->texture()->bind();
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(topLeft.x(), bottomRight.y());
            glTexCoord2f(0, 1); glVertex2f(topLeft.x(), topLeft.y());
            glTexCoord2f(1, 1); glVertex2f(bottomRight.x(), topLeft.y());
            glTexCoord2f(1, 0); glVertex2f(bottomRight.x(), bottomRight.y());
        glEnd();
        t->texture()->release();
    }
    
    Q_UNUSED(mvp);
    Q_UNUSED(screen);
    Q_UNUSED(viewport);
}

