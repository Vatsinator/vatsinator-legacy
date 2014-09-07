/*
 * airlinedatabase.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef AIRLINEDATABASE_H
#define AIRLINEDATABASE_H

#include <QObject>
#include <QString>
#include <QMap>

#include "singleton.h"
#include "ui/notifiable.h"

class Airline;

/**
 * The AirlineDatabase class is a layer between Vatsinator and the raw
 * database file.
 */
class AirlineDatabase : public QObject, public Notifiable, public Singleton<AirlineDatabase> {
  Q_OBJECT
  
public:
  
  /**
   * The default constructor.
   */
  AirlineDatabase(QObject* = nullptr);
  
  /**
   * Called by VatsinatorApplication only.
   */
  void initialize();
  
  /**
   * Searches for the airline record in the database.
   * Returns nullptr if nothing could be found.
   */
  Airline* find(const QString&);
  
  /**
   * Searches for the airline record in the database.
   * Returns nullptr if nothing could be found.
   */
  const Airline* find(const QString&) const;
  
  /**
   * Gives direct access to the map. It stores
   * ICAO code <-> record pointer pairs.
   */
  inline const QMap<QString, Airline*>& airlines() const { return __airlines; }
  
  /**
   * Returns the URL where airlies logos can be fetched from.
   */
  inline const QString& airlineLogoUrl() const { return __airlineLogoUrl; }
  
  /**
   * Indicates whether any airline logo can be fetched or not.
   */
  inline bool canFetch() const { return __canFetch; }
  
private:
  
  /* ICAO <-> instance pairs */
  QMap<QString, Airline*>       __airlines;
  
  QString       __airlineLogoUrl;
  bool          __canFetch;

};

#endif // AIRLINEDATABASE_H
