/*
 * airportitem.h
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

#ifndef AIRPORTITEM_H
#define AIRPORTITEM_H

#include <QObject>
#include <QVector>
#include <QColor>
#include <QPixmap>
#include <QStaticText>

#include "ui/map/mapitem.h"

class Airport;
class TmaArea;
class MapScene;

/**
 * The AirportItem class represents a single airport on the map.
 * 
 * \sa TmaArea.
 */
class AirportItem : public MapItem {
    Q_OBJECT
    
public:
    /**
     * Creates new AirportItem with the specified \c airport.
     * \c parent is passed to MapItem's constructor.
     */
    explicit AirportItem(const Airport* airport, QObject* parent = nullptr);
    
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
     * Gives direct access to the Airport object that this item represents
     * on the map.
     */
    inline const Airport* data() const
    {
        return __airport;
    }
    
    AirportItem() = delete;
    
private:
    void __loadIcon() const;
    void __drawLines(QPainter* painter, const WorldTransform& transform) const;
    
private slots:
    void __invalidate();

private:
    MapScene* __scene;
    const Airport* __airport; /**< Data pointer */
    mutable QPixmap __icon;
    TmaArea* __tma;
    
};

#endif // AIRPORTITEM_H
