/*
 * flightitem.h
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

#ifndef FLIGHTITEM_H
#define FLIGHTITEM_H

#include <QObject>
#include <QtOpenGL>
#include "ui/map/mapitem.h"

class Pilot;

class FlightItem : public QObject, public MapItem {
  Q_OBJECT

public:
  FlightItem(const Pilot*, QObject* = nullptr);
  FlightItem() = delete;
  
  virtual ~FlightItem();
  
  void drawModel() const;
  
  bool needsDrawing() const override;
  const QPointF& position() const override;
  QString tooltipText() const override;
  QMenu* menu(QWidget*) const override;
  void showDetailsWindow() const override;
  
  inline const Pilot* data() const { return __pilot; }
  
private:
  const Pilot*  __pilot;
  QPointF       __position;
  
  GLuint        __model;
};

#endif // FLIGHTITEM_H
