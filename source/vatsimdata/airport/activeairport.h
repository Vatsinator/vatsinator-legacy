/*
    activeairport.h
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


#ifndef ACTIVEAIRPORT_H
#define ACTIVEAIRPORT_H

#include <QString>

#include "vatsimdata/airport.h"

class ControllerTableModel;
class FlightTableModel;
class Pilot;

class ActiveAirport : public Airport {
  
public:
  ActiveAirport(QString = "");
  
  virtual ~ActiveAirport();
  
  unsigned countDepartures() const override;
  unsigned countOutbounds() const override;
  unsigned countArrivals() const override;
  unsigned countInbounds() const override;
  
  bool hasApproach() const override;
  Controller::Facilities facilities() const override;
  
  void addStaff(const Controller*);
  void addInbound(const Pilot*);
  void addOutbound(const Pilot*);
  
  inline ControllerTableModel *
  staffModel() const { return __staff; }
  
  inline FlightTableModel *
  inboundsModel() const { return __inbounds; }
  
  inline FlightTableModel *
  outboundsModel() const { return __outbounds; }
  
private:
  ControllerTableModel* __staff;
  FlightTableModel*     __inbounds;
  FlightTableModel*     __outbounds;
  
};

#endif // ACTIVEAIRPORT_H
