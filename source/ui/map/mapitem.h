/*
 * mapitem.h
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

#ifndef MAPITEM_H
#define MAPITEM_H

#include <QPointF>
#include <QString>

#include "vatsimdata/lonlat.h"

class QOpenGLShaderProgram;

/**
 * MapItem is an interface for any object that exists on the map.
 */
class MapItem {
public:
  
  /**
   * Indicates whether the item is visible on the map.
   */
  virtual bool isVisible() const = 0;
  
  /**
   * Indicates whether the item's label should be drawn on the map.
   * Note that returning false in this function does not mean drawlabel() will
   * not be called.
   */
  virtual bool isLabelVisible() const = 0;
  
  /**
   * Position of the item, global coordinates.
   */
  virtual const LonLat& position() const = 0;
  
  /**
   * Draws the specific item.
   * For FlightItems it will be the airplane model.
   * For AirportItems it will be the airport icon.
   * For FirItems it will be the label.
   * @param shader Shader that is in use during rendering the item.
   */
  virtual void drawItem(QOpenGLShaderProgram*) const = 0;
  
  /**
   * Draws the item label.
   * FirItem won't draw anything.
   * @param shader Shader that is in use during rendering the item.
   */
  virtual void drawLabel(QOpenGLShaderProgram*) const = 0;
  
  /**
   * Draws the item specific elements when it is under mouse.
   * For FlightItems and AirportItems it means just rendering the lines.
   * @param shader Shader that is in use during rendering the item.
   */
  virtual void drawFocused(QOpenGLShaderProgram*) const = 0;
  
  /**
   * Tooltip text, shown when the item is mouseover'ed.
   * Return empty string to ignore.
   */
  virtual QString tooltipText() const = 0;
  
  /**
   * Calls UserInterface::showDetails().
   */
  virtual void showDetails() const  = 0;
};

#endif // MAPITEM_H
