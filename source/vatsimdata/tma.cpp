/*
 * tma.cpp
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

#include "vatsimdata/airport.h"

#include "tma.h"

Tma::Tma(QString icao, QObject* parent) :
    QObject(parent),
    __icao(qMove(icao)) {}

Tma::Tma(QString icao, const QJsonArray& coords, QObject* parent) :
    Tma(icao, parent)
{
    __load(coords);
}

void
Tma::__load(const QJsonArray& coords)
{
    qreal y = qQNaN();
    
    for (const auto& value : coords) {
        Q_ASSERT(value.isDouble());
        
        if (qIsNaN(y))
            y = value.toDouble();
        else {
            double x = value.toDouble();
            __points << Point{static_cast<float>(x), static_cast<float>(y)};
            y = qQNaN();
        }
    }
    
    Q_ASSERT(qIsNaN(y));
}
