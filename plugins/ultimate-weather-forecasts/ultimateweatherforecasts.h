/*
 * ultimateweatherforecasts.h
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

#ifndef ULTIMATEWEATHERFORECASTS_H
#define ULTIMATEWEATHERFORECASTS_H

#include <QObject>
#include <QList>
#include <QQueue>
#include <QNetworkAccessManager>

#include "plugins/weatherforecastinterface.h"

class WeatherForecastReply;
class WeatherForecastRequest;

/**
 * The weather forecast plugin that uses George Vustrey's Ultimate Weather
 * Forecasts API, hosted on mashape.com.
 * https://www.mashape.com/george-vustrey/ultimate-weather-forecasts
 */
class UltimateWeatherForecasts : public QObject, public WeatherForecastInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.eu.vatsinator.Vatsinator.WeatherForecastInterface" FILE "ultimateweatherforecasts.json")
  Q_INTERFACES(WeatherForecastInterface)

public:
  UltimateWeatherForecasts();
  
  WeatherForecastReply* fetch(WeatherForecastRequest* request) override;
  
private:
  void __newRequest();
  void __finishRequest();
  
private slots:
  void __readyRead();
  void __finished();
  
private:
  QNetworkAccessManager __nam;
  QByteArray            __data;
  QNetworkReply*        __reply;
  
  QList<WeatherForecastReply*> __forecastReplies;
  QQueue<WeatherForecastReply*> __pendingForecasts;
  
};

#endif // ULTIMATEWEATHERFORECASTS_H
