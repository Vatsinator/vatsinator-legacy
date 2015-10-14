/*
 * firarea.h
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

#ifndef FIRAREA_H
#define FIRAREA_H

#include "ui/map/maparea.h"

class Fir;
class FirItem;
class MapScene;

/**
 * The FirArea class draws the Flight Information Region.
 */
class FirArea : public MapArea {
    Q_OBJECT

public:
    explicit FirArea(const Fir* fir, const FirItem* item, QObject* parent = nullptr);
    
    /**
     * \copydoc MapArea::boundingRect()
     */
    QRectF boundingRect() const override;
    
    /**
     * \copydoc MapArea::isVisible()
     */
    bool isVisible() const override;
    
    /**
     * \copydoc MapArea::draw()
     */
    void draw(QPainter* painter, const WorldTransform& transform) const override;
    
    /**
     * Gives direct access to the FIR that this area represents on the map.
     */
    inline const Fir* data() const
    {
        return __fir;
    }
    
private:
    MapScene* __scene;
    const Fir* __fir;
    const QRectF __rect;
    QVector<LonLat> __boundaries;
    const FirItem* __item;
    
};

#endif // FIRAREA_H
