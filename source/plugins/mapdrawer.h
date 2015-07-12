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

class QMatrix4x4;
class QRectF;

/**
 * MapDrawer provides an interface for drawing the world map.
 */
class MapDrawer {

public:
    /**
     * Flags describe plugin's requirements and specific behaviour.
     */
    enum MapDrawerFlag {
    
        RequireOpenGLContextOnInitialize = 0x1, /**< Indicates that the plugin
      requires a valid OpenGL context when initialize() is called. */

    };
    Q_DECLARE_FLAGS(MapDrawerFlags, MapDrawerFlag)
    
    /**
     * Defines the flags for the plugin.
     */
    virtual MapDrawerFlags flags() const = 0;
    
    /**
     * Initializes the plugin.
     *
     * This function is called just before the first frame is rendered.
     */
    virtual void initialize() = 0;
    
    /**
     * Renders a map.
     */
    virtual void draw(const QMatrix4x4& mvp, const QRectF& screen, qreal zoom) = 0;
    
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MapDrawer::MapDrawerFlags)

Q_DECLARE_INTERFACE(MapDrawer, "org.eu.vatsinator.Vatsinator.MapDrawer")

#endif // MAPDRAWER_H
