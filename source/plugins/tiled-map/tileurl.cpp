/*
 * tileurl.cpp
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

#include <QtCore>

#include "tileprovider.h"

#include "tileurl.h"

TileUrl::TileUrl() :
    __zoom(0)
{

}

TileUrl::TileUrl(quint64 x, quint64 y, quint64 zoom, TileProvider* provider) :
    __x(x),
    __y(y),
    __zoom(zoom),
    __url(provider->url(x, y, zoom))
{
    
}

bool
TileUrl::isValid() const
{
    return __zoom > 0;
}

bool
TileUrl::operator ==(const TileUrl& other) const
{
    return __x == other.__x && __y == other.__y && __zoom == other.__zoom;
}
