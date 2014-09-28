/*
 * weatherforecastrequest.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef WEATHERFORECASTREQUEST_H
#define WEATHERFORECASTREQUEST_H

#include <QString>
#include "vatsimdata/lonlat.h"

class WeatherForecastRequest {
  
public:
  
  /**
   * Creates a new weather forecast request for the given airport.
   * @param airport The airport ICAO code.
   */
  WeatherForecastRequest(QString);
  
  void setPosition(const LonLat&);
  void setCountry(const QString&);
  void setCity(const QString&);
  
  /**
   * Specifies the airport the weather forecast is requested for.
   */
  inline const QString& airport() const { return __airport; }
  
  /**
   * Specifies the precise position the weather forecast is fetched for.
   * It may be more precise than country and city.
   */
  inline const LonLat& position() const { return __position; }
  
  /**
   * Country name. City is always in the specified country.
   */
  inline const QString& country() const { return __country; }
  
  /**
   * City that the provider should fetch the weather forecast for.
   */
  inline const QString& city() const { return __city; }
  
private:
  QString __airport;
  LonLat __position;
  QString __country;
  QString __city;

};

#endif // WEATHERFORECASTREQUEST_H
