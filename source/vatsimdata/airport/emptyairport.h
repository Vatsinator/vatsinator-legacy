/*
    emptyairport.h
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


#ifndef EMPTYAIRPORT_H
#define EMPTYAIRPORT_H

#include <QMap>

#include "vatsimdata/airport.h"

class EmptyAirport : public Airport {
  
public:
  EmptyAirport(const QString&);
  EmptyAirport(const AirportRecord*);
  
  virtual ~EmptyAirport();
  
  inline unsigned
  countDepartures() const { return 0; }
  
  inline unsigned
  countOutbounds() const { return 0; }
  
  inline unsigned
  countArrivals() const { return 0; }
  
  inline unsigned
  countInbounds() const { return 0; }
  
  inline bool
  hasApproach() const { return false; }
  
  inline unsigned
  getFacilities() const { return 0; }
  
  inline void
  drawLines() const {}
  
};

#endif // EMPTYAIRPORT_H
