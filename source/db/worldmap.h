/*
    worldmap.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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


#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <QCoreApplication>
#include <QVector>

#include "db/point.h"
#include "ui/notifiable.h"
#include "singleton.h"

/**
 * The WorldMap class is a layer between the application and the database.
 * It contains global coastline as well as triangles.
 */
class WorldMap : public QObject, public Notifiable, public Singleton<WorldMap> {
  Q_OBJECT

  struct Polygon {
    QVector<Point>          borders;
    QVector<unsigned int>   triangles;
  };
  
public:
  
  /**
   * Default constructor.
   */
  WorldMap(QObject* parent = nullptr);
  
  /**
   * Read by VatsinatorApplication only.
   */
  void initialize();
  
  /**
   * Gives direct access to the coastline.
   */
  const QVector<Point>& borders() const { return __worldPolygon.borders; }
  
  /**
   * Gives direct access to triangles that fill lands.
   */
  const QVector<unsigned int>& triangles() const { return __worldPolygon.triangles; }

private:
  void __readDatabase();

  Polygon __worldPolygon;


};

#endif // WORLDMAP_H
