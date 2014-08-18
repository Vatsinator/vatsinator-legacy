/*
    pilot.h
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


#ifndef PILOT_H
#define PILOT_H

#include <QPointF>
#include <QStringList>
#include <QString>
#include <QVector>

#include "vatsimdata/client.h"

class Airport;
struct Point;

/**
 * This class contains info about one particular pilot - his
 * flight plan, actual position, plane, etc etc.
 * Note that in Vatsinator we use both "Pilot" or "Flight" for the same class.
 * This really needs to be fixed and we should stay consistent in our name
 * conventions. But as for now, don't be surprised when you meet "Flight"
 * anywhere in the code. It is about this exact class.
 */
class Pilot : public Client {  
  Q_OBJECT

public:
  /* Types */
  enum FlightRules {
    Ifr, Vfr
  };
  
  enum Phase {
    Departing,
    Airborne,
    Arrived
  };
  
  struct Pressure {
    QString ihg;
    QString mb;
  };
  
  /**
   * Route, as filed in the flight plan.
   * To get departure & destination airport pointers directly,
   * see origin() and destination() methods.
   */
  struct Route {
    QString origin;
    QString destination;
    QString route;
    QString altitude;
    
    /* This is vector of points that belong to the route.
     * The first point is the origin airport, the middle one is
     * the pilot position and the last one is the destination airport.
     */
    QVector<LonLat> waypoints;
  };
  
  Pilot() = delete;
  
  /**
   * @param list Data.
   * @param prefiled Indicates whether the flight is only prefiled.
   */
  Pilot(const QStringList&, bool = false);
  virtual ~Pilot();
  
  void update(const QStringList&);
  
  /**
   * Estimated Time of Arrival.
   */
  const QTime& eta() const;
  
  /**
   * Progress [1-100].
   */
  int progress() const;
  
  /**
   * The current altitude.
   */
  inline int altitude() const { return __altitude; }
  
  /**
   * The client's ground speed, in knots.
   */
  inline int groundSpeed() const { return __groundSpeed; }
  
  /**
   * The client's squawk code.
   * It's string, not int, as squawk might start with '0'.
   */
  inline const QString& squawk() const { return __squawk; }
  
  /**
   * The client's aircraft.
   */
  inline const QString& aircraft() const { return __aircraft; }
  
  /**
   * The position of the aircraft from previous update.
   * If this is the first update, current position is kept.
   */
  inline const LonLat& oldPosition() const { return __oldPosition; }
  
  /**
   * The client's current True Air Speed, in knots.
   * @sa groundSpeed().
   */
  inline int tas() const { return __tas; }
  
  /**
   * The client's flight rules - Ifr or Vfr.
   */
  inline const Pilot::FlightRules& flightRules() const { return __flightRules; }
  
  /**
   * The client's remarks.
   */
  inline const QString& remarks() const { return __remarks; }
  
  /**
   * Scheduled Time of Departure
   */
  inline const QTime& std() const { return __std; }
  
  /**
   * Actual Time of Departure
   */
  inline const QTime& atd() const { return __atd; }
  
  /**
   * Scheduled Time of Arrival
   */
  inline const QTime& sta() const { return __sta; }
  
  /**
   * The client's current heading.
   */
  inline unsigned heading() const { return __heading; }
  
  /**
   * The client's current flight phase.
   * Flight status is determined by the current altitude, speed and distance
   * from airport (origin or destination).
   * 
   * TODO Climbing & descending status options.
   */
  inline Pilot::Phase phase() const { return __phase; }
  
  /**
   * The client's baro setting.
   */
  inline const Pilot::Pressure& pressure() const { return __pressure; }
  
  /**
   * The client's route details. It includes planned origin and destination
   * airport, altitude and filled route.
   */
  inline const Pilot::Route& route() const { return __route; }
  
  /**
   * @return Origin airport or nullptr if could not match any.
   */
  inline const Airport* origin() const { return __origin; }
  
  /**
   * @return Destination airport or nullptr if could not match any.
   */
  inline const Airport* destination() const { return __destination; }
  
  /**
   * Prefiled only means that client has prefiled the flight plan, but
   * he did not log in yet.
   */
  inline bool isPrefiledOnly() const { return __prefiledOnly; }

private:
  /**
   * Adds origin/destination airports to the vectors.
   */
  void __updateAirports();
  
  /**
   * Checks whether pilot is departing, airborn or has just arrived.
   */
  void __discoverFlightPhase();
  
  /**
   * Checks whether the route crosses the IDL and fixes it.
   */
  void __fixupRoute();
  
  int                   __altitude;
  int                   __groundSpeed;
  QString               __squawk;
  QString               __aircraft;
  LonLat                __oldPosition;
  int                   __tas;
  Pilot::FlightRules    __flightRules;
  QTime                 __std; /* Scheduled Time of Departure  */
  QTime                 __atd; /* Actual Time of Departure */
  QTime                 __sta; /* Scheduled Time of Arrival */
  mutable QTime         __eta; /* Estimated Time of Arrival */
  mutable int           __progress; /* [1-100] */
  QString               __remarks;
  unsigned              __heading;
  Pilot::Phase          __phase;
  Pilot::Pressure       __pressure;
  Pilot::Route          __route;
  const Airport*        __origin;
  const Airport*        __destination;
  bool                  __prefiledOnly;

};

#endif // PILOT_H
