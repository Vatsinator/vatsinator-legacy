/*
 * tiledmapdrawer.h
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

#ifndef TILEDMAPDRAWER_H
#define TILEDMAPDRAWER_H

#include <QObject>

#include "plugins/mapdrawer.h"
#include "tileprovider.h"

class TileManager;

/**
 * A plugin that uses tiles to render the map.
 */
class TiledMapDrawer : public QObject, public MapDrawer {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.Vatsinator.MapDrawer")
    Q_INTERFACES(MapDrawer)

public:
    TiledMapDrawer(QObject *parent = nullptr);
    
    virtual ~TiledMapDrawer();
    
    /**
     * \copydoc MapDrawer::types()
     */
    QStringList types() const override;
    
    /**
     * \copydoc MapDrawer::initialize()
     */
    void initialize(MapRenderer* renderer) override;
    
    /**
     * \copydoc MapDrawer::draw()
     */
    void draw(QPainter* painter, const WorldTransform& transform) override;
    
private slots:
    void __updateMapType();
    
private:
    MapRenderer* __renderer;
    TileManager* __tiles;
    TileProvider __providers[2];
    
};

#endif // TILEDMAPDRAWER_H
