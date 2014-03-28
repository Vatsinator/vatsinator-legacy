/*
    fir.h
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


#ifndef FIR_H
#define FIR_H

#include <QVector>

#include "db/point.h"

#include "vatsimdata/airport.h"

class AirportTableModel;
class Controller;
class ControllerTableModel;
class FirDatabase;
class FlightTableModel;
class Pilot;
struct FirHeader;

class Fir {

public:
  Fir();
  virtual ~Fir();

  void addStaff(const Controller*);
  void addUirStaff(const Controller*);

  void addFlight(const Pilot*);

  void addAirport(const Airport*);

  void fixupName();

  void loadHeader(const FirHeader&);
  void clear();
  bool isStaffed() const;

  inline ControllerTableModel* staffModel() const { return __staff; }
  inline FlightTableModel* flightsModel() const { return __flights; }
  inline AirportTableModel* airportsModel() const { return __airports; }
  inline bool hasUirStaff() const { return __uirStaffCount; }
  inline const QString& icao() const { return __icao; }
  inline bool isOceanic() const { return __oceanic; }
  inline const Point& textPosition() const { return __textPosition; }
  inline const QString& name() const { return __name; }
  inline void setName(const QString& _n) { __name = _n; }
  inline const QString& country() const { return __country; }
  inline void setCountry(const QString& _c) { __country = _c; }
  inline QVector<Point>& borders() { return __borders; }
  inline const QVector<Point>& borders() const { return __borders; }
  inline QVector<unsigned short>& triangles() { return __triangles; }
  inline const QVector<unsigned short>& triangles() const { return __triangles; }

private:

  QString __icao;
  bool __oceanic;
  Point __externities[2];
  Point __textPosition;

  QString __name;
  QString __country;
  QVector<Point> __borders;
  QVector<unsigned short> __triangles;

  ControllerTableModel* __staff;
  FlightTableModel*     __flights;
  AirportTableModel*    __airports;

  unsigned __uirStaffCount;
};

#endif // FIR_H
