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

class Airline;

class AirlineDatabase : public QObject, public Singleton<AirlineDatabase> {
  Q_OBJECT
  
signals:
  
  /* Connected to UserInterface::warning() */
  void warning(QString);
  
public:
  AirlineDatabase(QObject* = nullptr);
  virtual ~AirlineDatabase();
  
  Airline* find(const QString&);
  const Airline* find(const QString&) const;
  
  inline const QMap<QString, Airline*>& airlines() const {
    return __airlines;
  }
  
  inline const QString& airlineLogoUrl() const {
    return __airlineLogoUrl;
  }
  
  inline bool canFetch() const {
    return __canFetch;
  }
  
private:
  void __init();
  
  /* ICAO <-> instance pairs */
  QMap<QString, Airline*>       __airlines;
  
  QString       __airlineLogoUrl;
  bool          __canFetch;

};

#endif // AIRLINEDATABASE_H
