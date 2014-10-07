/*
 * weatherforecastrequest.cpp
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

#include "weatherforecastrequest.h"

WeatherForecastRequest::WeatherForecastRequest(QString icao) :
    __airport(icao) {}

void
WeatherForecastRequest::setPosition(const LonLat& position) {
  __position = position;
}

void
WeatherForecastRequest::setCountry(const QString& country) {
  __country = country;
}

void
WeatherForecastRequest::setCity(const QString& city) {
  __city = city;
}
