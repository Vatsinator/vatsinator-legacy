/*
    pilot.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

struct Point;

class Pilot : public Client, public Clickable {

  /**
   * This class contains info about one particular pilot - his
   * flight plan, actual position, plane, etc etc.
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
  
  const QTime& eta() const;

  inline void
  drawLines() const { drawLineFrom(); drawLineTo(); }

  inline Client::Type
  clientType() const { return Client::PILOT; }
  
  inline Clickable::Type
  objectType() const { return Clickable::PLANE; }
  
  inline int
  altitude() const { return __altitude; }
  
  inline int
  groundSpeed() const { return __groundSpeed; }
  
  inline const QString &
  squawk() const { return __squawk; }
  
  inline const QString &
  aircraft() const { return __aircraft; }
  
  inline int
  tas() const { return __tas; }
  
  inline const Pilot::FlightRules &
  flightRules() const { return __flightRules; }
  
  /**
   * Scheduled Time of Departure
   */
  inline const QTime &
  std() const { return __std; }
  
  /**
   * Actual Time of Departure
   */
  inline const QTime &
  atd() const { return __atd; }
  
  /**
   * Scheduled Time of Arrival
   */
  inline const QTime &
  sta() const { return __sta; }
  
  inline const QString &
  remarks() const { return __remarks; }
  
  inline unsigned
  heading() const { return __heading; }
  
  inline Pilot::Status
  flightStatus() const { return __flightStatus; }
  
  inline const Pilot::Pressure &
  pressure() const { return __pressure; }
  
  inline const Pilot::Route &
  route() const { return __route; }
  
  inline GLuint
  modelTexture() const { return __modelTexture; }
  
  inline bool
  isPrefiledOnly() const { return __prefiledOnly; }

  inline GLuint
  callsignTip() const { return __callsignTip ? __callsignTip : __generateTip(); }
  
protected:
  int     __altitude;
  int     __groundSpeed;
  QString __squawk;
  QString __aircraft;
  
  /* True AirSpeed */
  int     __tas;

  /* IFR/VFR */
  Pilot::FlightRules __flightRules;

  QTime           __std; /* Scheduled Time of Departure  */
  QTime           __atd; /* Actual Time of Departure */
  QTime           __sta; /* Scheduled Time of Arrival */
  mutable QTime   __eta; /* Estimated Time of Arrival */
  QTime           __estimatedArrTime;
  QString         __remarks;
  unsigned        __heading;
  Pilot::Status   __flightStatus;
  Pilot::Pressure __pressure;
  Pilot::Route    __route;
  bool            __prefiledOnly;
  
  GLuint    __modelTexture;

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
  void __generateLines() const;

  /**
   * Generates the callsign label.
   */
  GLuint __generateTip() const;
  
  /**
   * Parses the NATs.
   * TODO: Include AIRAC, parse every route, not only NATs.
   */
  void __parseRoute() const;

  bool __isCrossingIDL(QVector<GLfloat>&) const;

  mutable QVector<GLfloat> __lineFrom;
  mutable QVector<GLfloat> __lineTo;
  
  /* Route parsing is pretty rich, avoid doing it if unnesesary */
  mutable bool __linesGenerated;

  mutable GLuint __callsignTip;
  
  /* Vector of coordinates */
  QVector<Point> __routePoints;

};

#endif // PILOT_H
