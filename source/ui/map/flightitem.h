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
class MapScene;
class Texture;

class FlightItem : public QObject, public MapItem {
  Q_OBJECT

public:
  
  /* Describes line type */
  enum LineType {
    OriginToPilot = 0x1,
    PilotToDestination = 0x2
  };
  Q_DECLARE_FLAGS(LineTypes, LineType);
  
  FlightItem(const Pilot*, QObject* = nullptr);
  FlightItem() = delete;
  
  virtual ~FlightItem();
  
  void drawModel(QOpenGLShaderProgram*) const;
  void drawLabel(QOpenGLShaderProgram*) const;
  void drawLines(LineTypes, QOpenGLShaderProgram*) const;
  
  bool needsDrawing() const override;
  const LonLat& position() const override;
  QString tooltipText() const override;
  QMenu* menu(QWidget*) const override;
  void showDetailsWindow() const override;
  
  inline const Pilot* data() const { return __pilot; }
  
private:
  void __initializeLabel() const;
  void __prepareLines() const;
  void __matchModel() const;
  
private slots:
  void __reloadSettings();
  void __invalidate();
  
private:
  MapScene*     __scene;
  const Pilot*  __pilot;
  LonLat        __position;
  
  mutable const Texture* __model;
  mutable QOpenGLTexture __label;
  
  mutable struct {
    QVector<GLfloat>    coords;
    QColor              color;
  } __otpLine, __ptdLine; // OriginToPilot & PilotToDestination
  
  mutable bool  __linesReady;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FlightItem::LineTypes);

#endif // FLIGHTITEM_H
