/*
 * componentsqmlplugin.cpp
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

#include "componentsqmlplugin.h"
#include "flightprogress.h"
#include "map.h"
#include "gui/mapscene.h"
#include <QtQml>

using namespace Vatsinator::Gui;

namespace Vatsinator { namespace Imports {

void ComponentsQmlPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QStringLiteral("org.eu.vatsinator.Components"));
    
    qmlRegisterType<FlightProgress>(uri, 1, 0, "FlightProgress");
    qmlRegisterType<Map>(uri, 1, 0, "Map");
}

}} /* namespace Vatsinator::Imports */
