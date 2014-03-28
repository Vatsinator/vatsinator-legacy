/*
    airport.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AIRPORT_H
#define AIRPORT_H

#include <QString>
#include <QPair>

#include "vatsimdata/client/controller.h"

struct AirportRecord;
class Fir;

class Airport {
  
  /*
   * This is the interface for an airport object.
   * It represents an airport on the map - can be clicked
   * or have the sub-menu.
   * Inherited by ActiveAirport and EmptyAirport classes.
   */

public:
  Airport(const QString&);
  Airport(const AirportRecord*);
  
  virtual ~Airport() = default;
  
  virtual unsigned countDepartures() const = 0;
  virtual unsigned countOutbounds() const = 0;
  virtual unsigned countArrivals() const = 0;
  virtual unsigned countInbounds() const = 0;
  
  virtual bool hasApproach() const = 0;
  virtual Controller::Facilities facilities() const = 0;
  
  inline const AirportRecord* data() const { return __data; }
  inline const QPair<Fir*, Fir*>& firs() const { return __firs; }
  inline QPair<Fir*, Fir*>& firs() { return __firs; }
  
private:
  const AirportRecord*  __data;
  QPair<Fir*, Fir*>     __firs;

};

#endif // AIRPORT_H
