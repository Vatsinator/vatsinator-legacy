/*
 * mapdrawer.h
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

#ifndef GUI_MAPDRAWER_H
#define GUI_MAPDRAWER_H

#include "gui/worldpainter.h"
#include "gui/v8rgui_export.h"

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * MapDrawer is an interface for drawing the world map.
 */
class V8RGUI_EXPORT MapDrawer {
public:
    /**
     * Draws the map onto the painter.
     */
    virtual void draw(WorldPainter* painter) = 0;

    virtual ~MapDrawer() = default;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_MAPDRAWER_H
