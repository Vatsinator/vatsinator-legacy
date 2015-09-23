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
 * The FirItem class represents a single FIR label on the map.
 * 
 * \sa FirArea.
 */
class FirItem : public MapItem {
    Q_OBJECT
    
public:
    /**
     * Constructs a new FirItem with the given \c fir. \c parent is passed
     * to QObject's constructor.
     */
    explicit FirItem(const Fir* fir, QObject* parent = nullptr);
    
    /**
     * \copydoc MapItem::isVisible()
     */
    bool isVisible() const override;
    
    /**
     * \copydoc MapItem::position()
     * Returns position of the label.
     */
    LonLat position() const override;
    
    /**
     * \copydoc MapItem::draw()
     */
    void draw(QPainter* painter, const WorldTransform& transform, DrawFlags flags) const override;
    
    /**
     * Gives direct access to the FIR that this item represents on the map.
     */
    inline const Fir* data() const
    {
        return __fir;
    }

    FirItem() = delete;
    
private slots:
    void __prepareLabel();
    
private:
    MapScene* __scene;
    const Fir* __fir;
    LonLat __position;
    mutable QStaticText __label;
    QFont __font;
    QVector<LonLat> __boundaries;
    
};

#endif // FIRITEM_H
