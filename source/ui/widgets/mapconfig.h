/*
    mapconfig.h
    This file specifies the configuration for the map.
    
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAPCONFIG_H
#define MAPCONFIG_H

// below/above these values the user can't zoom the map
#define ZOOM_MINIMUM                  1
#define ZOOM_MAXIMUM                  1500
#define ACTUAL_ZOOM_MINIMUM           0
#define ZOOM_NORMALIZE_COEFFICIENT    0.5
#define ZOOM_BASE                     1.1

// the range below which the mouse is in the object's
#define OBJECT_TO_MOUSE   0.03

// for these values the GL's context rendering size reprezents
// (-1.0; 1.0) orthogonal ranges for both x and y axes
#define BASE_SIZE_WIDTH   800
#define BASE_SIZE_HEIGHT  600

#define PILOTS_LABELS_FONT_COLOR    0, 0, 0
#define AIRPORTS_LABELS_FONT_COLOR  250, 250, 250
#define FIRS_LABELS_FONT_COLOR      173, 173, 173

// camera position won't go any further
#define RANGE_Y   1.0

// fonts properties
#define PILOT_FONT_PIXEL_SIZE   10
#define PILOT_FONT_WEIGHT       QFont::Normal
#define AIRPORT_FONT_PIXEL_SIZE 8
#define AIRPORT_FONT_WEIGHT     QFont::Bold
#define FIR_FONT_PIXEL_SIZE     10
#define FIR_FONT_WEIGHT         QFont::Bold

#endif // MAPCONFIG_H
