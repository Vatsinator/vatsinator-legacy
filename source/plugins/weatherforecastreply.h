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
  Q_ENUMS(ForecastError)
  
signals:
  void finished();

public:
  
  /**
   * Defines error that occured for that reply.
   */
  enum ForecastError {
    
    /* No error */
    NoError,
    
    /* The weather forecast could not be found for the given location */
    NotFoundError,
    
    /* A network error has occured */
    NetworkError
  };
  
  explicit WeatherForecastReply(WeatherForecastRequest*, QObject* = nullptr);
  
  virtual ~WeatherForecastReply();
  
  /**
   * The WeatherForecastReply takes ownership over the WeatherData.
   */
  void appendWeatherData(WeatherData*);
  
  /**
   * Set isFinished() status to the given value.
   * If true and the request was not finished before (isFinished() == false),
   * the finished() signal will be emitted.
   * @param finished
   */
  void setFinished(bool);
  
  /**
   * Sets the error status to the given value.
   */
  void setError(ForecastError);
  
  inline const WeatherForecastRequest* request() const { return __request; }
  inline const QVector<WeatherData*>& data() const { return __data; }
  inline bool isFinished() const { return __finished; }
  inline ForecastError error() const { return __error; }
  
private:
  WeatherForecastRequest* __request;
  QVector<WeatherData*> __data;
  bool __finished;
  ForecastError __error;
  
};

#endif // WEATHERFORECASTREPLY_H
