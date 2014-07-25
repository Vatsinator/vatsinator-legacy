/*
    airporttracker.h
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


#ifndef AIRPORTTRACKER_H
#define AIRPORTTRACKER_H

#include <QMap>
#include <QObject>

#include "singleton.h"

class Airport;

/**
 * TODO: remove (deprecated)
 */
class AirportTracker :
     public QObject,
     public Singleton<AirportTracker> {
  
  Q_OBJECT
  
public:
  AirportTracker(QObject* = 0);
  virtual ~AirportTracker();
  
  void updateData();
  
  inline const QMap< QString, const Airport* > &
  tracked() { return __trackedAirports; }
  
  inline bool
  isInitialized() { return __isInitialized; }
  
 private:
   void __readSettings();
   
   QMap< QString, const Airport* > __trackedAirports;
   bool __isInitialized;
   
private slots:
  void __toggleAirport(const Airport*);
  
};

#endif // AIRPORTTRACKER_H
