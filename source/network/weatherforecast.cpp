/*
 * weatherforecast.cpp
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

#include <QtNetwork>

#include "network/ultimate-weather-forecasts.h"

#include "vatsinatorapplication.h"

#include "weatherforecast.h"
#include "defines.h"

WeatherForecast::WeatherForecast(QObject* _parent) :
    QObject(_parent),
    __reply(nullptr) {}

void
WeatherForecast::fetchForecast(const QString& _city, const QString& _country) {
  if (__reply) {
    __reply->abort();
    __reply->deleteLater();
  }
  __temp.clear();
  
  QString location = _city % ", " % _country;
  
  QString url = QString(ULTIMATE_WEATHER_FORECASTS_API_URL).arg(location);
  QNetworkRequest request(url);
  request.setRawHeader("X-Mashape-Authorization", ULTIMATE_WEATHER_FORECASTS_API_KEY);
  
  VatsinatorApplication::log("Request: %s", qPrintable(request.url().toString()));
  
  __reply = __nam.get(request);
  connect(__reply, SIGNAL(finished()), this, SLOT(__finished()));
  connect(__reply, SIGNAL(readyRead()), this, SLOT(__readyRead()));
}

void
WeatherForecast::__readyRead() {
  __temp.append(__reply->readAll());
}

void
WeatherForecast::__finished() {
  __reply->deleteLater();
  __reply = nullptr;
  
  VatsinatorApplication::log("Response: %s", qPrintable(__temp));
}


