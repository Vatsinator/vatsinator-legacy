/*
 * geo.h
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

#ifndef CORE_GEO_H
#define CORE_GEO_H

#include "lonlat.h"
#include <QtGlobal>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * 
 * Returns distance between the two points, in nautical miles.
 */
qreal __VtrCoreApi__ nmDistance(const LonLat& a, const LonLat& b);

}} /* namespace Vatsinator::Core */

#endif // CORE_GEO_H
