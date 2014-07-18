/*
 * weatherforecastreply.h
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

#ifndef WEATHERFORECASTREPLY_H
#define WEATHERFORECASTREPLY_H

#include <QObject>
#include <QVector>

class RequestFinishedEvent;
class WeatherData;
class WeatherForecastRequest;

class WeatherForecastReply : public QObject {
  Q_OBJECT
  
signals:
  void finished();

public:
  explicit WeatherForecastReply(WeatherForecastRequest*, QObject* = nullptr);
  
  virtual ~WeatherForecastReply();
  
  /**
   * The WeatherForecastReply takes ownership over the WeatherData.
   */
  void appendWeatherData(WeatherData*);
  
  bool event(QEvent*) override;
  
  inline const WeatherForecastRequest* request() const { return __request; }
  inline const QVector<WeatherData*>& data() const { return __data; }
  
protected:
  virtual bool requestFinishedEvent(RequestFinishedEvent*);
  
private:
  WeatherForecastRequest* __request;
  QVector<WeatherData*> __data;
  
};

#endif // WEATHERFORECASTREPLY_H
