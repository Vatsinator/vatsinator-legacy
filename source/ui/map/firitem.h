/*
 * firitem.h
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

#ifndef FIRITEM_H
#define FIRITEM_H

#include <QObject>

#include "ui/map/mapitem.h"

class Fir;
class MapScene;

/**
 * The FirItem class represents a single FIR on the map.
 *
 * In the case of FirItem, position() is a position of label. drawBackground()
 * and drawBorders() methods are used directly by MapRenderer and they use
 * VBOs and VAOs to draw a lot of triangles in the most efficient way.
 */
class FirItem : public MapItem {
    Q_OBJECT
    
public:
    /**
     * Constructs a new FirItem with the given _fir_. _parent_ is passed
     * to QObject's constructor.
     */
    explicit FirItem(const Fir* fir, QObject* parent = nullptr);
    FirItem() = delete;
    
    virtual ~FirItem();
    
    bool isVisible() const override;
    bool isLabelVisible() const override;
    const LonLat& position() const override;
    void draw(QPainter* painter, const QTransform& transform) const override;
    QString tooltipText() const override;
    void showDetails() const override;
    
    /**
     * Gives direct access to the FIR that this item represents on the map.
     */
    inline const Fir* data() const
    {
        return __fir;
    }

private:
    MapScene*  __scene;
    const Fir* __fir;
    LonLat     __position;
    
    
};

#endif // FIRITEM_H
