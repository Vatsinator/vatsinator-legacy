/*
 * mapdrawerplugin.h
 * Copyright (C) 2017  Michał Garapich <michal@garapich.pl>
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


#ifndef GUI_MAPDRAWERPLUGIN_H
#define GUI_MAPDRAWERPLUGIN_H

#include "guiexport.h"
#include "mapdrawer.h"
#include "maprenderer.h"
#include <QtPlugin>

namespace Vatsinator { namespace Gui {

class __VtrGuiApi__ MapDrawerPlugin {
public:
    virtual MapDrawer* create(MapRenderer* renderer) = 0;
};

}} /* namespace Vatsinator::Gui */

Q_DECLARE_INTERFACE(Vatsinator::Gui::MapDrawerPlugin, "org.eu.vatsinator.vtrgui.MapDrawerPlugin")

#endif // GUI_MAPDRAWERPLUGIN_H
