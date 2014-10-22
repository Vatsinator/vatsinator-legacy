/*
    fir.h
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


#ifndef FIR_H
#define FIR_H

#include <QObject>
#include <QString>

class Airport;
class AirportTableModel;
class Controller;
class ControllerTableModel;
class FirDatabase;
class FlightTableModel;
class Pilot;
struct FirRecord;

/**
 * Keeps information about a single FIR.
 */
class Fir : public QObject {
  Q_OBJECT
  
signals:
  void updated();
  
public:
  
  /**
   * \param record Record in the database.
   */
  Fir(const FirRecord* record);
  
  virtual ~Fir();
  
  /**
   * Records staff in the fir.
   */
  void addStaff(const Controller* atc);
  
  /**
   * Records UIR staff in the fir.
   * The UIR staff is not taken into account when isStaffed() and
   * isEmpty() methods are called.
   */
  void addUirStaff(const Controller* atc);
  
  /**
   * Records flight in the fir.
   */
  void addFlight(const Pilot* pilot);
  
  /**
   * Adds Airport to the FIR.
   */
  void addAirport(const Airport* airport);
  
  /**
   * Having the name set, this method suffixes it with "Oceanic" or "Center",
   * appropriately.
   */
  void fixupName();
  
  /**
   * \return True if any ATC is available (either this FIR or containing UIR).
   */
  bool isStaffed() const;
  
  /**
   * \return True if there is no ATC controlling the FIR.
   */
  bool isEmpty() const;
  
  /**
   * \return True is position is not equal to (0, 0).
   */
  bool hasValidPosition() const;
  
  void setName(const QString& name);
  void setCountry(const QString& country);
  
  inline const FirRecord* data() const { return __data; }
  
  inline ControllerTableModel* staff() const { return __staff; }
  inline ControllerTableModel* uirStaff() const { return __uirStaff; }
  inline FlightTableModel* flights() const { return __flights; }
  inline AirportTableModel* airports() const { return __airports; }
  
  inline const QString& icao() const { return __icao; }
  inline bool isOceanic() const { return __oceanic; }
  
  inline const QString& name() const { return __name; }
  inline const QString& country() const { return __country; }
  
private:
  
  const FirRecord* __data;

  QString       __icao;
  bool          __oceanic;

  QString       __name;
  QString       __country;

  ControllerTableModel* __staff;
  ControllerTableModel* __uirStaff;
  FlightTableModel*     __flights;
  AirportTableModel*    __airports;
};

#endif // FIR_H
