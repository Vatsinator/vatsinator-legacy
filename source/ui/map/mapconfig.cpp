/*
 * mapconfig.cpp
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

#include <QtGui>

#include "mapconfig.h"

QString MapConfig::generalizedDensity()
{
    qreal dpi = QGuiApplication::primaryScreen()->physicalDotsPerInch();
    if (dpi <= 160.0)
        return QStringLiteral("mdpi");
    else if (dpi <= 240.0)
        return QStringLiteral("hdpi");
    else if (dpi <= 320.0)
        return QStringLiteral("xhdpi");
    else if (dpi <= 480.0)
        return QStringLiteral("xxhdpi");
    else
        return QStringLiteral("xxxhdpi");
}
