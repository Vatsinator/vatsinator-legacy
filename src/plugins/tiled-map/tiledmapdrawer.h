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
#include "gui/maprenderer.h"

namespace TiledMapDrawer {

class TileRenderer;

/**
 * \ingroup Tiled-map-drawer
 * @{
 */
class TiledMapDrawer : public Vatsinator::Gui::MapDrawer {
public:
    /**
     * Creates a new TiledMapDrawer instance.
     */
    explicit TiledMapDrawer(Vatsinator::Gui::MapRenderer* renderer);

    /**
     * Destroys this TiledMapDrawer instance.
     */
    virtual ~TiledMapDrawer();
    
    /**
     * \copydoc Vatsinator::Gui::MapDrawer::draw()
     */
    void draw(Vatsinator::Gui::WorldPainter* painter) override;
    
private:
    QScopedPointer<TileRenderer> m_tileRenderer;
    Vatsinator::Gui::MapRenderer* m_renderer;
    
}; /** @} */

} /* namespace TiledMapDrawer */

#endif // TILEDMAPDRAWER_H
