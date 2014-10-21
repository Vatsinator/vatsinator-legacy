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
#include "ui/notifiable.h"

class Airline;

/**
 * The AirlineDatabase class is a layer between Vatsinator and the raw
 * database file. The database is automatically sunchronized with the
 * VatsinatorDatabase instance by ResourceManager.
 */
class AirlineDatabase : public QObject, public Notifiable {
  Q_OBJECT
  
public:
  
  /**
   * The default constructor.
   */
  AirlineDatabase(QObject* parent = nullptr);
  
  /**
   * Called by VatsinatorApplication only. Reads the database file.
   */
  void initialize();
  
  /**
   * Looks for the airline record in the database.
   * 
   * \param icao The lookup ICAO code of the target airline.
   * \return Airline instance pointer or _nullptr_ if nothing could be found.
   */
  Airline* find(const QString& icao);
  
  /**
   * Looks for the airline record in the database.
   * 
   * \param icao The lookup ICAO code of the target airline.
   * \return Airline instance pointer or _nullptr_ if nothing could be found.
   */
  const Airline* find(const QString& icao) const;
  
  /**
   * Gives direct access to the map that stores ICAO code <-> instance pairs.
   */
  inline const QMap<QString, Airline*>& airlines() const { return __airlines; }
  
  /**
   * Provies the base URL for all airlines' logos.
   * The URL is stored in the database file.
   */
  inline const QString& airlineLogoUrl() const { return __airlineLogoUrl; }
  
  /**
   * Indicates whether any airline logo can be fetched or not.
   * To provide any tools of remote management, this property can be set to
   * false in the database file. All airline logos won't be fetched then.
   */
  inline bool canFetchLogos() const { return __canFetch; }
  
private:
  
  /* ICAO <-> instance pairs */
  QMap<QString, Airline*>       __airlines;
  
  QString       __airlineLogoUrl;
  bool          __canFetch;

};

#endif // AIRLINEDATABASE_H
