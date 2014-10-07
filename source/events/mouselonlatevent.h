/*
 * mouselonlatevent.h
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

#ifndef MOUSELONLATEVENT_H
#define MOUSELONLATEVENT_H

#include <QEvent>

#include "vatsimdata/lonlat.h"
#include "events/types.h"

/**
 * The MouseLonLatEvent class is used to indicate events of mouse that
 * are connected with longitude and latitude change. It is used by MapWidget
 * class to notify mouse move events on the map.
 */
class MouseLonLatEvent : public QEvent {

public:
  /**
   * Constructs the new event from the given _point_.
   * 
   * \param point The point on the map.
   */
  MouseLonLatEvent(const LonLat& point);
  
  /**
   * Gets the current position of the mouse, mapped to longitude and latitude
   * coordinates.
   */
  inline const LonLat& lonLat() const { return __lonLat; }

private:
  LonLat __lonLat;
  
};

#endif // MOUSELONLATEVENT_H
