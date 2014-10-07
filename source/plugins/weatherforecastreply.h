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
#include <QList>

class RequestFinishedEvent;
class WeatherData;
class WeatherForecastRequest;

/**
 * The WeatherForecastReply class contains data for request response that was
 * created by WeatherForecastInterface.
 */
class WeatherForecastReply : public QObject {
  Q_OBJECT
  Q_ENUMS(ForecastError)
  
  /**
   * Indicates state of the reply.
   * If value of this property is true it means that the reply is ready to use.
   */
  Q_PROPERTY(bool finished READ isFinished WRITE setFinished NOTIFY finished)
  
  /**
   * Specifies error that may have happened during the request.
   */
  Q_PROPERTY(ForecastError error READ error)
  
signals:
  /**
   * The finished() signal is emitted when all the data is fetched and ready.
   */
  void finished();

public:
  /**
   * Defines error that occured for that reply.
   */
  enum ForecastError {
    NoError,            /**< Indicates that no error has occured */
    NotFoundError,      /**< The weather forecast could not be found for the given location */
    NetworkError        /**< A network error has occured */
  };
  
  /**
   * Constructs new reply for corresponding _request_ and passes _parent_
   * to the QObject.
   */
  explicit WeatherForecastReply(WeatherForecastRequest* request, QObject* parent = nullptr);
  
  virtual ~WeatherForecastReply();
  
  /**
   * Adds WeatherData item to the response data.
   * The WeatherForecastReply takes ownership over the _data_.
   */
  void appendWeatherData(WeatherData* data);
  
  /**
   * Sets the error status to the given value.
   */
  void setError(ForecastError error);
  
  void setFinished(bool finished);
  
  /**
   * Gives access to the corresponding request.
   */
  inline const WeatherForecastRequest* request() const { return __request; }
  
  /**
   * Gets parsed weather forecast data.
   */
  inline const QList<WeatherData*>& data() const { return __data; }

  inline ForecastError error() const { return __error; }  
  inline bool isFinished() const { return __finished; }
  
private:
  WeatherForecastRequest* __request;
  QList<WeatherData*> __data;
  bool __finished;
  ForecastError __error;
  
};

#endif // WEATHERFORECASTREPLY_H
