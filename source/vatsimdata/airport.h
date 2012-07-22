/*
    airport.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include <QVector>
#include <QString>

#include <GL/gl.h>

#include "ui/mapwidget/clickable.h"

struct AirportRecord;
class Controller;
class ControllerTableModel;
class FlightTableModel;
class Fir;
class Pilot;

class Airport : public Clickable {

  /**
   * This class represents the active airport object.
   */

public:

  /**
   * Default ctor.
   * @param icao Airport ICAO code.
   */
  Airport(const QString& = "");

  /**
   * Frees the memory.
   */
  virtual ~Airport();

  /**
   * Adds the ATC.
   * @param c Pointer to ATC instance.
   */
  void addStaff(const Controller*);

  /**
   * Adds inbound/outbound flight.
   * @param p Pointer to the pilot instance.
   */
  void addInbound(const Pilot*);
  void addOutbound(const Pilot*);

  /**
   * Some info functions.
   */
  unsigned countDepartures() const;
  unsigned countOutbounds() const;
  unsigned countArrivals() const;
  unsigned countInbounds() const;

  bool hasApproach() const;

  unsigned getFacilities() const;

  void drawLines() const;

  inline ControllerTableModel*
  getStaffModel() const { return __staff; }

  inline FlightTableModel*
  getInboundsModel() const { return __inbounds; }

  inline FlightTableModel*
  getOutboundsModel() const { return __outbounds; }

  inline const AirportRecord*
  getData() const { return __data; }

  inline ObjectType
  objectType() const { return AIRPORT; }

  inline GLuint
  getLabelTip() const { return __labelTip ? __labelTip : __generateTip(); }

  inline Fir**
  getFirs() { return __firs; }


private:
  GLuint __generateTip() const;

  const AirportRecord*  __data;

  ControllerTableModel* __staff;

  FlightTableModel* __inbounds;
  FlightTableModel* __outbounds;


  mutable GLuint __labelTip;

  Fir* __firs[2];


};

#endif // AIRPORT_H
