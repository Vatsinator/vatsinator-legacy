/*
 * paletteprovider.cpp
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

#include "paletteprovider.h"

PaletteProvider::PaletteProvider(QObject* parent) : QObject (parent)
{

}

QColor
PaletteProvider::byHue(int hue) const
{
    switch (hue) {
        case 50: return QColor(225,245,254);
        case 100: return QColor(179,229,252);
        case 200: return QColor(129,212,250);
        case 300: return QColor(79,195,247);
        case 400: return QColor(41,182,246);
        case 500: return QColor(3,169,244);
        case 600: return QColor(3,155,229);
        case 700: return QColor(2,136,209);
        case 800: return QColor(2,119,189);
        case 900: return QColor(1,87,155);
        
        case 1100: return QColor(255,138,128);
        case 1200: return QColor(255,82,82);
        case 1400: return QColor(255,23,68);
        case 1700: return QColor(213,0,0);
        
        default: return QColor();
    }
}

QColor
PaletteProvider::background() const
{
    return QColor(255, 255, 255);
}

QColor
PaletteProvider::divider() const
{
    return QColor(250, 250, 250);
}
