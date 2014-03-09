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

class MapItem {
public:
  
  /**
   * Position of the item, global coordinates.
   */
  virtual const QPointF& position() const = 0;
  
  /**
   * Tooltip text, shown when the item is mouseover'ed.
   * Return empty string to ignore.
   */
  virtual QString tooltipText() const = 0;
  
  /**
   * Popup menu that is shown when user right-clicks on
   * the item.
   * 
   * @param parent Parent widget for the menu to be passed in the ctor.
   */
  virtual QMenu* menu(QWidget*) const = 0;
  
  /**
   * Open window that shows the item's data details.
   */
  virtual void showDetailsWindow() const  = 0;
};

#endif // MAPITEM_H
