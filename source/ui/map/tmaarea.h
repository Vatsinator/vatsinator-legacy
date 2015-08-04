/*
 * tmaarea.h
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

#ifndef TMAAREA_H
#define TMAAREA_H

#include "ui/map/maparea.h"

class Airport;
class MapScene;

class TmaArea : public MapArea {
    Q_OBJECT
    
public:
    explicit TmaArea(const Airport* airport, QObject *parent = nullptr);
    
    QRectF boundingRect() const override;
    
    bool isVisible() const override;
    
    void draw(QPainter *painter, const WorldTransform &transform) const override;
    
private:
    void __initialize(const Airport* airport);
    void __boundingRectFromBoundaries();
    
    MapScene* __scene;
    QRectF __rect;
    QVector<LonLat> __boundaries;
};

#endif // TMAAREA_H
