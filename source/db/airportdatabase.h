/*
    airportdatabase.h
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

#ifndef AIRPORTDATABASE_H
#define AIRPORTDATABASE_H

#include <QObject>
#include <QVector>

#include "singleton.h"
#include "ui/notifiable.h"

/*
 * This struct represents a single raw airport entry in the database file.
 */
#pragma pack(1)
struct AirportRecord {

  char  name[128];
  char  city[128];
  char  country[128];
  char  iata[8];
  char  icao[8];
  int   is_fir_a_oceanic;
  char  fir_a[8];
  int   is_fir_b_oceanic;
  char  fir_b[8];
  float latitude;
  float longitude;
  int   altitude;

};
#pragma pack()

/*
 * AirportDatabase is a layer between Vatsinator and the raw database.
 */
class AirportDatabase : public QObject, public Notifiable, public Singleton<AirportDatabase> {
  Q_OBJECT
  
public:

  /**
   * Default ctor.
   */
  AirportDatabase(QObject* = nullptr);
  
  /**
   * Called by VatsinatorApplication only.
   */
  void initialize();
  
  /**
   * Looks for the airport.
   * @param icao ICAO code.
   * @return nullptr if nothing found.
   */
  const AirportRecord* find(const QString&);
  
  /**
   * Gives direct access to the airpors vector.
   */
  inline QVector<AirportRecord>& airports() { return __airports; }
  
  /**
   * Gives direct access to the airports vector.
   */
  inline const QVector<AirportRecord>& airports() const { return __airports; }

private:
  
  /**
   * Reads the entire Airport database.
   */
  void __readDatabase();

  QVector<AirportRecord> __airports;
  
};

#endif // AIRPORTDATABASE_H
