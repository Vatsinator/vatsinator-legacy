/*
 * weatherforecastdownloader.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef WEATHERFORECASTDOWNLOADER_H
#define WEATHERFORECASTDOWNLOADER_H

#include <QObject>

class WeatherForecastInterface;
class WeatherForecastReply;
class WeatherForecastRequest;

/**
 * The WeatherForecastDownloader class is responsible for downloading an
 * accurate weather forecasts.
 * Downloading weather forecasts is a chain of responsibility - until one
 * of the plugins is not able to download required data, they are all queried
 * sequentially.
 */
class WeatherForecastDownloader : public QObject {
  Q_OBJECT
  
signals:
  /**
   * Signal is emitted when weather forecast is downloaded.
   */
  void finished(WeatherForecastReply* reply);

public:
  WeatherForecastDownloader(QObject* parent = nullptr);
  
  /**
   * Tries to download the specified forecast using each plugin until at least
   * one of them succeeds.
   * 
   * \param request Specifies desired weather forecast.
   */
  void download(WeatherForecastRequest* request);
  
private:
  void __useCurrentProvider();
  
private slots:
  void __replyFinished();
  
private:
  
  static void __initializeProviders();
  
  WeatherForecastRequest* __request;
  QList<WeatherForecastInterface*>::iterator __currentProvider;
  
  static QList<WeatherForecastInterface*> __providers;
  static bool __initialized;
  
};

#endif // WEATHERFORECASTDOWNLOADER_H
