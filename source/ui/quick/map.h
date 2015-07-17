/*
 * map.h
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#ifndef MAP_H
#define MAP_H

#include <QQuickPaintedItem>

class MapRenderer;
class QQuickWindow;

/**
 * The Map class is an exposed to QML map surface. It provides useful methods
 * for the qml file, such as zoom and position handling.
 */
class Map : public QQuickPaintedItem {
    Q_OBJECT
    
signals:
    /**
     * Emitted when the item is ready to render the map.
     */
    void ready();
    
public:
    /**
     * Default constructor, passes _parent_ to the QQuickItem.
     */
    Map(QQuickItem * parent = nullptr);
    
    /**
     * Destructor.
     */
    virtual ~Map();
    
    /**
     * Updates zoom by the given _factor_.
     */
    Q_INVOKABLE void updateZoom(qreal factor);
    
    /**
     * Updates the map position.
     */
    Q_INVOKABLE void updatePosition(int x, int y);
    
    /**
     * Returns an absolute path to where the cached map image is stored.
     * 
     * \note It is not guaranteed that the image file exists.
     */
    Q_INVOKABLE QString cachedImageSource() const;
    
    void paint(QPainter* painter) override;
    
    /**
     * Gives direct access to the map renderer instance.
     */
    inline MapRenderer* renderer()
    {
        return __renderer;
    }
    
private:
    MapRenderer* __renderer;
    
};

#endif // MAP_H
