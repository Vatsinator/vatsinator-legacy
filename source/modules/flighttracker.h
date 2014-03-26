/*
    flighttracker.h
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


#ifndef FLIGHTTRACKER_H
#define FLIGHTTRACKER_H

#include <QObject>
#include <QString>

#include "singleton.h"

class Pilot;

class FlightTracker :
    public QObject,
    public Singleton<FlightTracker> {

  /**
   * This class is for flight tracking - it centres the map on the
   * tracked plane after every update.
   */

  Q_OBJECT

public:
  FlightTracker(QObject* = 0);

  void updateData();

  inline const Pilot *
  tracked() { return __currentPointer; }

private:
  QString       __currentCallsign;
  const Pilot*  __currentPointer;

private slots:
  void __trackFlight(const Pilot*);
  void __trackFlight(const Pilot*, int);
  void __cancelFlight();
};

#endif // FLIGHTTRACKER_H
