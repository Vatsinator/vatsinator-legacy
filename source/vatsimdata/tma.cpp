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

#include <chrono>
#include <QtCore>
#include "PolygonTriangulator/PolygonTriangulator.h"

#include "vatsimdata/airport.h"

#include "tma.h"

Tma::Tma(QString icao, QObject* parent) :
    QObject(parent),
    __icao(qMove(icao)),
    __triangulated(false) {}

Tma::Tma(QString icao, const QJsonArray& coords, QObject* parent) :
    Tma(icao, parent)
{
    __load(coords);
}

void
Tma::triangulate()
{
    auto start = std::chrono::high_resolution_clock::now();
    
    std::pair<std::vector<double>, std::vector<double>> coords;
    
    for (auto& p : __points) {
        coords.first.push_back(static_cast<double>(p.x));
        coords.second.push_back(static_cast<double>(p.y));
    }
    
    PolygonTriangulator triangulator(coords.first, coords.second);
    
    if (triangulator.triangles()->size() == 0) {
        qWarning("Failed triangulating TMA for %s", qPrintable(__icao));
        return;
    }
    
    for (auto t : *triangulator.triangles()) {
        /*
         * Dunno why, but PolygonTriangulator returns indices counting from 1, not
         * from 0.
         * As we don't have extremely large and complex polygons, unsigned
         * short is enough to store our indices.
         */
        __triangles << static_cast<quint16>(t.at(0)) - 1
                    << static_cast<quint16>(t.at(1)) - 1
                    << static_cast<quint16>(t.at(2)) - 1;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    qDebug() << "Tma::triangulate():" << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "nanoseconds";
    
    emit triangulated();
}

Tma*
Tma::circle(const Airport* airport)
{
    Tma* tma = new Tma(QStringLiteral("ZZZZ"));
    
    tma->__points << Point { static_cast<float>(airport->position().x()), static_cast<float>(airport->position().y()) };
    
    for (qreal angle = 0.0; angle < (2 * M_PI); angle += 0.1) {
        float x = qCos(angle) + airport->position().x();
        float y = 0.5f * qSin(angle) + airport->position().y();
        tma->__points << Point{x, y};
    }
    
    tma->__points << Point { static_cast<float>(airport->position().x()) + 1.0f, static_cast<float>(airport->position().y()) };
    
    return tma;
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
