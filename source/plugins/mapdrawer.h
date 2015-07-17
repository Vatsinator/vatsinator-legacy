/*
 * mapdrawer.h
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

#ifndef MAPDRAWER_H
#define MAPDRAWER_H

#include <QtPlugin>
#include "ui/map/maprenderer.h"

class QMatrix4x4;
class QRectF;
class QSize;

/**
 * MapDrawer provides an interface for drawing the world map.
 */
class MapDrawer {

public:
    /**
     * Destructor.
     */
    virtual ~MapDrawer() = default;
    
    /**
     * Initializes the plugin.
     *
     * This function is called just before the first frame is rendered.
     */
    virtual void initialize(MapRenderer* renderer) = 0;
    
    /**
     * Renders a map.
     */
    virtual void draw(QPainter* painter, const QTransform& transform) = 0;
    
};

Q_DECLARE_INTERFACE(MapDrawer, "org.eu.vatsinator.Vatsinator.MapDrawer")

#endif // MAPDRAWER_H
