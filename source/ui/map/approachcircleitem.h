/*
 * approachcircleitem.h
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

#ifndef APPROACHCIRCLEITEM_H
#define APPROACHCIRCLEITEM_H

#include <QtOpenGL>
#include <QObject>

#include "ui/map/mapitem.h"

class Airport;

class ApproachCircleItem : public QObject, public MapItem {
  Q_OBJECT

public:
  ApproachCircleItem(const Airport*, QObject* = nullptr);
  ApproachCircleItem() = delete;
  
  void drawCircle() const;
  
  bool needsDrawing() const override;
  const LonLat& position() const override;
  QString tooltipText() const override;
  QMenu* menu(QWidget*) const override;
  void showDetailsWindow() const override;
  
private:
  void __fillColors() const;
  
private slots:
  void __reloadSettings();
  
private:
  LonLat                        __position;
  mutable QVector<GLfloat>      __colors;
  
  static QVector<GLfloat>       __circle;
  
};

#endif // APPROACHCIRCLEITEM_H
