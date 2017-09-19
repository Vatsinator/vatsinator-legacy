/*
 * mapaddon.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#ifndef GUI_MAPADDON_H
#define GUI_MAPADDON_H

#include "gui/worldpainter.h"
#include "gui/worldtransform.h"
#include "gui/v8rgui_export.h"
#include <QtCore/QObject>
#include <QtGui/QPainter>

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 *
 * An interface for any map addon.
 */
class V8RGUI_EXPORT MapAddon {
public:
    /**
     * This method is called just before all the map items (airports,
     * airplanes, etc.) are drawn.
     */
    virtual void drawBeforeItems(WorldPainter* painter) = 0;

    /**
     * This method is called right after all the map items are drawn.
     */
    virtual void drawAfterItems(WorldPainter* painter) = 0;

}; /** @} */

}} /* namespace Vatsinator::Gui */

Q_DECLARE_INTERFACE(Vatsinator::Gui::MapAddon, "org.eu.vatsinator.v8rgui.MapAddon")

#endif // GUI_MAPADDON_H
