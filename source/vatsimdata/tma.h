/*
 * tma.h
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

#ifndef TMA_H
#define TMA_H

#include <QObject>
#include <QVector>
#include "db/point.h"

class Tma : public QObject {
  Q_OBJECT
  
  Q_PROPERTY(QString icao READ icao)
  Q_PROPERTY(bool loaded READ isLoaded NOTIFY loaded)
  Q_PROPERTY(bool triangulated READ isTriangulated NOTIFY triangulated)
  
signals:
  void loaded();
  void triangulated();
  
public:
  Tma(QString icao, QObject* parent = nullptr);
  Tma(QString icao, const QJsonArray& coords, QObject* parent = nullptr);
  
  void load();
  void triangulate();
  
  inline const QVector<Point>& points() const { return __points; }
  inline const QVector<quint32>& triangles() const { return __triangles; }
  
  inline const QString& icao() const { return __icao; }
  inline bool isLoaded() const { return __loaded; }
  inline bool isTriangulated() const { return __triangulated; }
  
private:
  void __load(const QJsonArray& coords);
  
  QString __icao;
  bool __loaded;
  bool __triangulated;
  
  QVector<Point> __points;
  QVector<quint32> __triangles;
  
};

#endif // TMA_H
