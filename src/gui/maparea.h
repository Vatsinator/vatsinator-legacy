/*
 * maparea.h
 * Copyright (C) 2015 Michał Garapich <michal@garapich.pl>
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

#ifndef GUI_MAPAREA_H
#define GUI_MAPAREA_H

#include "guiexport.h"
#include "mapdrawable.h"
#include <QRectF>

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The \c MapArea class is a base for all map objects that are represented
 * as areas - for example FIRs or approach areas around airports.
 */
class __VtrGuiApi__ MapArea : public MapDrawable {
    Q_OBJECT

public:
    /**
     * \copydoc MapDrawable::MapDrawable
     */
    explicit MapArea(MapScene* scene, QObject* parent = nullptr);
    
    /**
     * Defines the outer bounds of the area. The MapScene uses this
     * value to determine whether the given area is visible on the
     * screen.
     */
    virtual QRectF boundingRect() const = 0;

}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_MAPAREA_H
