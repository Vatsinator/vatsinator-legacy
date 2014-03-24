/*
 * airportitem.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef AIRPORTITEM_H
#define AIRPORTITEM_H

#include <QObject>
#include <QtOpenGL>

#include "ui/map/mapitem.h"

class Airport;

class AirportItem : public QObject, public MapItem {
  Q_OBJECT

public:
  AirportItem(const Airport*, QObject* = nullptr);
  AirportItem() = delete;
  
  virtual ~AirportItem();
  
  void drawIcon() const;
  void drawLabel() const;
  
  const QPointF& position() const override;
  QString tooltipText() const override;
  QMenu* menu(QWidget*) const override;
  void showDetailsWindow() const override;
  
  inline const Airport* data() const { return __airport; }
  
private:
  void __makeIcon() const;
  void __generateLabel() const;

private slots:
  void __resetLabel();
  
private:
  const Airport* __airport;
  QPointF        __position;
  
  mutable GLuint __icon;
  mutable GLuint __label;
  
};

#endif // AIRPORTITEM_H
