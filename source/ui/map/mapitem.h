/*
 * mapitem.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef MAPITEM_H
#define MAPITEM_H

#include <QObject>
#include <QString>
#include <QPainter>

#include "ui/map/worldtransform.h"
#include "vatsimdata/lonlat.h"

/**
 * The MapItem is a base class for map objects that are points. Each item has
 * its \c position that determines whether it needs to be drawn or not.
 * The \c isVisible() method is an additional requirement that can eliminate
 * items from being drawn. For example, all \c AirportItems that represent
 * an empty airport will be hidden (with the default settings). The
 * \c tooltipText() method returns a string for the tooltip that is visible
 * whether user puts the mouse cursor over the item. The \c showDetails()
 * method opens new view represeting the particular item's details.
 * 
 * To show areas on the map, look for \c MapArea class.
 */
class MapItem : public QObject {
    Q_OBJECT
    Q_ENUMS(DrawFlag)
    
public:
    /**
     * Flags for drawing a single item on the map.
     */
    enum DrawFlag {
        DrawSelected    = 0x1 /* Indicates that the item should be drawn as
                                it was selected (i.e. by mouse) */,
    };
    Q_DECLARE_FLAGS(DrawFlags, DrawFlag)
    
    
    /**
     * The default constructor. Passes \c parent to the QObject.
     */
    explicit MapItem(QObject* parent = nullptr);
    
    /**
     * Indicates whether the item is visible on the map or not.
     */
    virtual bool isVisible() const = 0;
    
    /**
     * Position of the item, global coordinates.
     */
    virtual LonLat position() const = 0;
    
    /**
     * Draws the item on the painter.
     */
    virtual void draw(QPainter* painter, const WorldTransform& transform, DrawFlags flags) const = 0;
    
    /**
     * Tooltip text, shown when the item is mouseover'ed.
     * Return empty string to ignore.
     */
    Q_INVOKABLE virtual QString tooltipText() const = 0;
    
    /**
     * Calls UserInterface::showDetails().
     */
    virtual void showDetails() const  = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MapItem::DrawFlags)

#endif // MAPITEM_H
