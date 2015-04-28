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

#include "tma.h"

Tma::Tma(QString icao, QObject* parent) :
    QObject(parent),
    __icao(qMove(icao)),
    __loaded(false),
    __triangulated(false) {}

Tma::Tma(QString icao, const QJsonArray& coords, QObject* parent) :
    Tma(icao, parent) {
  __load(coords);
}

void
Tma::load() {
  /* TODO Load coordinates on-demand */
  Q_UNREACHABLE();
}

void
Tma::triangulate() {
  auto start = std::chrono::high_resolution_clock::now();
  
  std::pair<std::vector<double>, std::vector<double>> coords;
  for (auto& p: __points) {
    coords.first.push_back(p.x);
    coords.second.push_back(p.y);
  }
  
  PolygonTriangulator triangulator(coords.first, coords.second);
  if (triangulator.triangles()->size() == 0) {
    qWarning("Failed triangulating TMA for %s", qPrintable(__icao));
    return;
  }
  
  for (auto t: *triangulator.triangles()) {
    __triangles << t.at(0) << t.at(1) << t.at(2);
  }
  
  auto end = std::chrono::high_resolution_clock::now();
  qDebug() << "Tma::triangulate():" << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << "nanoseconds";
  
  emit triangulated();
}

void
Tma::__load(const QJsonArray& coords) {
  qreal x = qQNaN();
  
  for (const auto& value: coords) {
    Q_ASSERT(value.isDouble());
    if (qIsNaN(x)) {
      x = value.toDouble();
    } else {
      double y = value.toDouble();
      __points << Point{static_cast<float>(x), static_cast<float>(y)};
      x = qQNaN();
    }
  }
  
  Q_ASSERT(qIsNaN(x));
  __loaded = true;
  emit loaded();
}
