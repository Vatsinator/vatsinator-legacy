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
#include <QMenu>

#include "vatsimdata/lonlat.h"

/**
 * MapItem is an interface for any object that exists on the map.
 */
class MapItem {
public:
  
  /**
   * Indicates whether the items needs to be drawn or not.
   */
  virtual bool needsDrawing() const = 0;
  
  /**
   * Position of the item, global coordinates.
   */
  virtual const LonLat& position() const = 0;
  
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
