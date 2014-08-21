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
  
  bool isVisible() const override;
  bool isLabelVisible() const override;
  const LonLat& position() const override;
  void drawItem(QOpenGLShaderProgram*) const override;
  void drawLabel(QOpenGLShaderProgram*) const override;
  void drawFocused(QOpenGLShaderProgram*) const override;
  QString tooltipText() const override;
  void showDetails() const override;
  
  inline const Airport* data() const { return __airport; }
  
private:
  void __getColor() const;
  
private slots:
  void __reloadSettings();
  
private:
  const Airport*                __airport;
  LonLat                        __position;
  mutable QColor                __color;
  
  static QVector<GLfloat>       __circle;
  
};

#endif // APPROACHCIRCLEITEM_H
