/*
 * flightitem.h
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

#ifndef FLIGHTITEM_H
#define FLIGHTITEM_H

#include <QObject>
#include <QPixmap>
#include "ui/map/mapitem.h"

class Pilot;
class MapScene;

/**
 * The FlightItem class represents a single Pilot on the map.
 */
class FlightItem : public MapItem {
    Q_OBJECT
    
public:
    /**
     * The constructor takes \c pilot as a data provider of what to show
     * on the map. Data is updated automatically.
     */
    explicit FlightItem(const Pilot* pilot, QObject* parent = nullptr);
    
    /**
     * \copydoc MapItem::isVisible()
     */
    bool isVisible() const override;
    
    /**
     * \copydoc MapItem::position()
     */
    LonLat position() const override;
    
    /**
     * \copydoc MapItem::draw()
     */
    void draw(QPainter* painter, const WorldTransform& transform, DrawFlags flags) const override;
    
    /**
     * \copydoc MapItem::tooltipText()
     */
    QString tooltipText() const override;
    
    /**
     * \copydoc MapItem::showDetails()
     */
    void showDetails() const override;
    
    /**
     * Gives direct access to the Pilot class instance that this item
     * represents on the map.
     */
    inline const Pilot* data() const
    {
        return __pilot;
    }
    
    FlightItem() = delete;
    
private:
    void __dropShadow(QPixmap* image) const;
    
    MapScene* __scene;
    const Pilot* __pilot;
    mutable QPixmap __model; /**< Normal aircraft model */
    mutable QPixmap __modelSelected; /**< Aircraft model when selected */
    
};

#endif // FLIGHTITEM_H
