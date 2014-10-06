/*
    airport.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include <QObject>
#include <QString>
#include <QPair>

#include "vatsimdata/client/controller.h"

struct AirportRecord;
class ControllerTableModel;
class Fir;
class FlightTableModel;
class Pilot;

/**
 * The Airport class represents a single airport.
 */
class Airport : public QObject {
  Q_OBJECT
  
signals:
  void updated();

public:
  
  /**
   * \deprecated
   */
  Airport(const QString& icao);
  
  /**
   * \param data Record in the database.
   */
  Airport(const AirportRecord* record);
  
  ~Airport();
  
  /**
   * Counts flights that are about to take off.
   * \param includePrefiled If false, this method will not count prefiled flights.
   *    Default: true.
   */
  unsigned countDepartures(bool includePrefiled = true) const;
  
  /**
   * Counts flights that originate from the airport and are airborne
   * or have just arrived.
   */
  unsigned countOutbounds() const;
  
  /**
   * Counts flights that have just landed.
   */
  unsigned countArrivals() const;
  
  /**
   * Counts flights that fly towards the airport.
   */
  unsigned countInbounds() const;
  
  /**
   * Returns OR-combined facilities that are available on the airport.
   */
  Controller::Facilities facilities() const;
  
  /**
   * Adds new controller to the airport.
   */
  void addStaff(const Controller* atc);
  
  /**
   * Adds new inbound flight to the airport.
   */
  void addInbound(const Pilot* pilot);
  
  /**
   * Adds new outbound flight to the airport.
   */
  void addOutbound(const Pilot* pilot);
  
  /**
   * \return True if the airport does not have any controllers or flights, otherwise false.
   */
  bool isEmpty() const;
  
  /**
   * \return True if at least one ATC is available on the airport.
   */
  bool isStaffed() const;
  
  /**
   * \return The airport's position.
   */
  LonLat position() const;
  
  /**
   * \return Pointer to AirportRecord in the database.
   */
  inline const AirportRecord* data() const { return __data; }
  
  /**
   * \return The airport ICAO code.
   */
  inline const QString& icao() const { return __icao; }
  
  /**
   * \return Staff model of the airport.
   */
  inline ControllerTableModel* staff() const { return __staff; }
  
  /**
   * \return Inbound flights for the airport.
   */
  inline FlightTableModel* inbounds() const { return __inbounds; }
  
  /**
   * \return Outbound flights for the airport.
   */
  inline FlightTableModel* outbounds() const { return __outbounds; }
  
private:
  const AirportRecord*  __data;
  const QString         __icao;
  
  ControllerTableModel* __staff;
  FlightTableModel*     __inbounds;
  FlightTableModel*     __outbounds;

};

#endif // AIRPORT_H
