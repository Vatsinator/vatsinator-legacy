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

#include <QOpenGLTexture>
#include <QObject>
#include <QVector>
#include <QColor>

#include "ui/map/mapitem.h"

class Airport;
class ApproachCircleItem;
class MapScene;

/**
 * The AirportItem class represents a single airport on the map.
 */
class AirportItem : public MapItem {
  Q_OBJECT

public:
  /**
   * Creates new AirportItem with the specified _airport_.
   * _parent_ is passed to MapItem's constructor.
   */
  explicit AirportItem(const Airport* airport, QObject* parent = nullptr);
  AirportItem() = delete;
  
  virtual ~AirportItem();
  
  bool isVisible() const override;
  bool isLabelVisible() const override;
  const LonLat& position() const override;
  void drawItem(QOpenGLShaderProgram* shader) const override;
  void drawLabel(QOpenGLShaderProgram* shader) const override;
  void drawFocused(QOpenGLShaderProgram* shader) const override;
  QString tooltipText() const override;
  void showDetails() const override;
  
  /**
   * Gives direct access to the Airport object that this item represents
   * on the map.
   */
  inline const Airport* data() const { return __airport; }
  
  inline const ApproachCircleItem* approachCircle() const { return __approachCircle; }
  
private:
  void __takeIcon() const;
  void __prepareLines() const;
  void __initializeLabel() const;

private slots:
  void __reloadSettings();
  void __invalidate();
  
private:
  MapScene*             __scene;
  const Airport*        __airport;
  LonLat                __position;
  ApproachCircleItem*   __approachCircle;
  
  mutable QOpenGLTexture* __icon;
  mutable QOpenGLTexture  __label;
  
  mutable struct {
    QVector<GLfloat>    coords;
    QColor              color;
  } __otpLines, __ptdLines; // OriginToPilot & PilotToDestination
  
  mutable bool  __linesReady;
  
};

#endif // AIRPORTITEM_H
