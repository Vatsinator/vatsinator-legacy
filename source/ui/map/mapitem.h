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

#include "vatsimdata/lonlat.h"

/**
 * MapItem is a base class for any object that exists on the map.
 */
class MapItem : public QObject {
    Q_OBJECT
    
public:
    explicit MapItem(QObject* parent = nullptr);
    
    /**
     * Indicates whether the item is visible on the map or not.
     */
    virtual bool isVisible() const = 0;
    
    /**
     * Indicates whether the item's label should be drawn on the map.
     * Note that returning false in this function does not mean drawlabel() will
     * not be called.
     */
    virtual bool isLabelVisible() const = 0;
    
    /**
     * Position of the item, global coordinates.
     */
    virtual const LonLat& position() const = 0;
    
    /**
     * Draws the item on the painter.
     */
    virtual void draw(QPainter* painter, const QTransform& transform) const = 0;
    
    /**
     * Tooltip text, shown when the item is mouseover'ed.
     * Return empty string to ignore.
     */
    virtual QString tooltipText() const = 0;
    
    /**
     * Calls UserInterface::showDetails().
     */
    virtual void showDetails() const  = 0;
};

#endif // MAPITEM_H
