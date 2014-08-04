/*
 * mapconfig.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include <QtGui>

#include "mapconfig.h"

#ifndef Q_OS_ANDROID
const QGLFormat&
MapConfig::glFormat()
{
    static QGLFormat fmt(
        QGL::DoubleBuffer
        | QGL::DepthBuffer
        | QGL::Rgba
        | QGL::AlphaChannel
        | QGL::SampleBuffers
    );
    return fmt;
}
#endif

const QString&
MapConfig::emptyAirportIcon()
{
    static QString icon(":/pixmaps/airport_inactive.png");
    return icon;
}

const QString&
MapConfig::activeAirportIcon()
{
    static QString icon(":/pixmaps/airport.png");
    return icon;
}

const QString&
MapConfig::activeStaffedAirportIcon()
{
    static QString icon(":/pixmaps/airport_staffed.png");
    return icon;
}

const QColor&
MapConfig::pilotPen()
{
    static QColor pen(0, 0, 0);
    return pen;
}

const QColor&
MapConfig::airportPen()
{
    static QColor pen(250, 250, 250);
    return pen;
}

const QImage&
MapConfig::pilotLabelBackground()
{
    static QImage label(":/pixmaps/pilot_tooltip.png");
    return label;
}

const QImage&
MapConfig::airportLabelBackground()
{
    static QImage label(":/pixmaps/airport_tooltip.png");
    return label;
}

const QImage&
MapConfig::firLabelBackground()
{
    static QImage label(64, 32, QImage::Format_ARGB32_Premultiplied);
    static bool filled = false;
    
    if (!filled) {
        label.fill(0);
        filled = true;
    }
    
    return label;
}

const QFont&
MapConfig::pilotFont()
{
    static QFont font("Verdana", 7, QFont::Normal);
    return font;
}

const QFont&
MapConfig::airportFont()
{
    static QFont font("Verdana", 6, QFont::Bold);
    return font;
}

const QFont&
MapConfig::firFont()
{
    static QFont font("Verdana", 8, QFont::Bold);
    return font;
}
