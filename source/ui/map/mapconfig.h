/*
 * mapconfig.h
 * Copyright (C) 2013-2015  Michał Garapich <michal@garapich.pl>
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

#ifndef MAPCONFIG_H
#define MAPCONFIG_H

#include <QtGlobal> // qreal
#include <QImage>

class QColor;
class QFont;
class QPixmap;

class MapConfig {
public:

    static constexpr qreal longitudeMax()
    {
        return 180.0;
    }
    
    static constexpr qreal latitudeMax()
    {
        return 85.0511;
    }
    
    /* Distance between mouse and object that object is considered
       to be under mouse, in pixels. */
    static constexpr qreal mouseOnObject()
    {
        return 15.0;
    }
    
    /* User won't be able to zoom below this value */
    static constexpr int zoomMinimum()
    {
        return 1;
    }
    
    /* Per-step zoom intensivity */
    static constexpr int zoomCoefficient()
    {
        return 30;
    }
    
    /* User won't be able to zoom above this value */
    static constexpr int zoomMaximum()
    {
        return 1500;
    }
    
    static constexpr qreal zoomNormalizeCoef()
    {
        return 0.5;
    }
    
    static constexpr qreal zoomBase()
    {
        return 1.1;
    }
    
    static constexpr qreal zoomDefault()
    {
#ifdef Q_OS_ANDROID
        return 3.0;
#else
        return 1.0;
#endif
    }
    
    /**
     * Returns prefix path for pixmaps that should be used or
     * the current device, depending on the DPI.
     * 
     * Possible values:
     * * mdpi
     * * hdpi
     * * xhdpi
     * * xxhdpi
     * * xxxhdpi
     */
    static QString generalizedDensity();
    
    /* Map layers */
    enum MapLayers {
        WorldMap = 1,
        UnstaffedFirs = 2,
        StaffedUirs = 3,
        StaffedFirs = 4,
        ApproachAreas = 5,
        
        EmptyAirports = 7,
        ActiveAirports = 8,
        
        Lines = 10,
        
        Pilots = 12,
        
        Count
    };
    
    MapConfig() = delete;
};

#endif // MAPCONFIG_H
