/*
 * tiledmapdrawer.h
 * Copyright (C) 2016  Michał Garapich
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

#include "gui/mapdrawer.h"
#include <QObject>

class TileRenderer;

/**
 * \defgroup Tiled-mad-drawer
 * @{
 *
 * A tile-based world map drawer plugin for Vatsinator.
 */

/**
 * The TiledMapDrawer is an implementation of MapDrawer that uses map tiles
 * to draw the world.
 */
class TiledMapDrawer : public QObject, public Vatsinator::Gui::MapDrawer {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.vtrgui.MapDrawer" FILE "tiledmapdrawer.json")
    Q_INTERFACES(Vatsinator::Gui::MapDrawer)

public:
    /**
     * Creates a new TiledMapDrawer instance, passes \c parent to the QObject's
     * constructor.
     */
    TiledMapDrawer(QObject* parent = nullptr);

    /**
     * Destroys this TiledMapDrawer instance.
     */
    virtual ~TiledMapDrawer();
    
    /**
     * \copydoc Vatsinator::Gui::MapDrawer::initialize()
     */
    void initialize(Vatsinator::Gui::MapRenderer* renderer) override;
    
    /**
     * \copydoc Vatsinator::Gui::MapDrawer::draw()
     */
    void draw(Vatsinator::Gui::WorldPainter* painter) override;
    
private slots:
    void updateMap(QRectF coords, QPixmap map);
    
private:
    TileRenderer* m_tileRenderer;
    Vatsinator::Gui::MapRenderer* m_renderer;
    
    QPixmap m_map;
    QRectF m_coords; /**< Area that is covered by rendered map, global coordinates */
    
};

/** @} */

#endif // TILEDMAPDRAWER_H
