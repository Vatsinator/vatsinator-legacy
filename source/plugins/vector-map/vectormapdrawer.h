/*
 * vectormapdrawer.h
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

#ifndef VECTORMAPDRAWER_H
#define VECTORMAPDRAWER_H

#include <QObject>

#include "plugins/mapdrawer.h"

/**
 * The VectorMapDrawer draws a vector, legacy map using data provided by
 * NSAA.
 */
class VectorMapDrawer : public QObject, public MapDrawer {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.Vatsinator.MapDrawer")
    Q_INTERFACES(MapDrawer)
    
public:
    VectorMapDrawer(QObject* parent = nullptr);
    
    MapDrawer::MapDrawerFlags flags() const override;
    void initialize() override;
    void draw(const QRectF& screen, qreal zoom) override;
    
};

#endif // VECTORMAPDRAWER_H
