/*
 * mapitem.h
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

#ifndef GUI_MAPITEM_H
#define GUI_MAPITEM_H

#include "guiexport.h"
#include "mapdrawable.h"
#include "core/lonlat.h"

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The MapItem is a base class for map objects that are points.
 * 
 * The MapItem class represents a single point on the map. Each item has
 * its \c position() that determines whether it needs to be drawn or not.
 * 
 * To show areas on the map, look for \c MapArea class.
 */
class __VtrGuiApi__ MapItem : public MapDrawable {
    Q_OBJECT

public:
    /**
     * \copydoc MapDrawable::MapDrawable
     */
    explicit MapItem(MapScene* scene, QObject* parent = nullptr);
    
    /**
     * Specifies the position of the item.
     */
    virtual Core::LonLat position() const = 0;
    
    /**
     * Returns size of graphical representation of this item.
     */
    virtual QSize size() const = 0;

    /**
     * Returns the z-attribute of the item.
     */
    virtual int z() const = 0;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

Q_DECLARE_METATYPE(Vatsinator::Gui::MapItem*)

#endif // GUI_MAPITEM_H
