/*
 * mapstate.h
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

#ifndef MAPSTATE_H
#define MAPSTATE_H

#include <QMetaType>

#include "vatsimdata/lonlat.h"

/**
 * The MapState class is used to specify current map state. It stores
 * current map position and zoom and tracked client and airports.
 * Modifying the state doesn't mean the map will update automatically.
 * States can be swapped, cached or stored. To apply the state, use
 * MapWidget::setState() method on the MapWidget singleton instance.
 * You can also obtain current map state by calling MapWidget::scene()
 * function.
 */
class MapState {
  
public:
  
  /**
   * Default constructor defines default map state - map center is
   * at (0.0, 0.0), zoom is at minimum, no pilots or airports are tracked.
   */
  MapState();
  
  /**
   * Copy costructor.
   */
  MapState(const MapState&) = default;
  
  /**
   * Copies the other scene to this one.
   */
  MapState& operator =(const MapState&) = default;
  
  /**
   * Sets the new center position.
   * @param center The new center point.
   */
  void setCenter(const LonLat&);
  
  /**
   * Sets the new zoom.
   * @param zoom The new zoom.
   */
  void setZoom(float);
  
  /**
   * Gets the map center position.
   */
  inline const LonLat& center() const { return __center; }
  
  /**
   * Gets the map center position.
   */
  inline LonLat& center() { return __center; }
  
  /**
   * Gets the map zoom.
   */
  inline float zoom() const { return __zoom; }
  
private:
  LonLat __center;
  float  __zoom;
  
};

Q_DECLARE_METATYPE(MapState)
QDataStream& operator<<(QDataStream&, const MapState&);
QDataStream& operator>>(QDataStream&, MapState&);

#endif // MAPSTATE_H
