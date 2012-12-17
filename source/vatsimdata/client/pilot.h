/*
    pilot.h
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


#ifndef PILOT_H
#define PILOT_H

#include <QStringList>
#include <QString>

#include <QtOpenGL>

#include "vatsimdata/clickable.h"
#include "vatsimdata/client.h"

class Pilot : public Client, public Clickable {

  /**
   * This class contains info about one particular pilot - his
   * flight plan, actual position, plane, etc etc.
   * TODO: Flight plan parsing (starting by NATs, may even include the AIRAC later).
   */

public:
  /* Types */
  enum FlightRules {
    IFR, VFR
  };
  
  enum Status {
    DEPARTING,
    AIRBORNE,
    ARRIVED
  };
  
  struct Pressure {
    QString ihg;
    QString mb;
  };
  
  struct Route {
    QString origin;
    QString destination;
    QString route;
    QString altitude;
  };
  
  Pilot() = delete;
  
  /**
   * @param list Data.
   * @param prefiled Indicates whether the flight is only prefiled.
   */
  Pilot(const QStringList&, bool = false);
  virtual ~Pilot();

  void drawLineFrom() const;
  void drawLineTo() const;

  inline void
  drawLines() const { drawLineFrom(); drawLineTo(); }

  inline Client::Type
  clientType() const { return Client::PILOT; }
  
  inline Clickable::Type
  objectType() const { return Clickable::PLANE; }
  
  inline int
  getAltitude() const { return __altitude; }
  
  inline int
  getGroundSpeed() const { return __groundSpeed; }
  
  inline const QString &
  getSquawk() const { return __squawk; }
  
  inline const QString &
  getAircraft() const { return __aircraft; }
  
  inline int
  getTas() const { return __tas; }
  
  inline const Pilot::FlightRules &
  getFlightRules() const { return __flightRules; }
  
  inline const QString &
  getRemarks() const { return __remarks; }
  
  inline unsigned
  getHeading() const { return __heading; }
  
  inline Pilot::Status
  getFlightStatus() const { return __flightStatus; }
  
  inline const Pilot::Pressure &
  getPressure() const { return __pressure; }
  
  inline const Pilot::Route &
  getRoute() const { return __route; }
  
  inline GLuint
  getModelTexture() const { return __modelTexture; }
  
  inline bool
  isPrefiledOnly() const { return __prefiledOnly; }

  inline GLuint
  getCallsignTip() const { return __callsignTip ? __callsignTip : __generateTip(); }
  
protected:
  int     __altitude;
  int     __groundSpeed;
  QString __squawk;

  QString __aircraft;

  /* True AirSpeed */
  int     __tas;

  /* IFR/VFR */
  Pilot::FlightRules __flightRules;

  QString   __remarks;

  unsigned  __heading;
  
  Pilot::Status   __flightStatus;
  
  Pilot::Pressure __pressure;

  Pilot::Route    __route;

  GLuint    __modelTexture;

  bool      __prefiledOnly;

private:
  /**
   * Adds origin/destination airports to the vectors.
   */
  void __updateAirports();
  
  /**
   * Checks whether pilot is departing, airborn or has just arrived.
   */
  void __setMyStatus();

  /**
   * Generates in/out lines.
   */
  void __generateLines();

  /**
   * Generates the callsign label.
   */
  GLuint __generateTip() const;

  GLfloat* __lineFrom;
  GLfloat* __lineTo;

  mutable GLuint __callsignTip;

};

#endif // PILOT_H
