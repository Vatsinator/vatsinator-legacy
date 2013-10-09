/*
 * weatherforecast.h
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

#ifndef WEATHERFORECAST_H
#define WEATHERFORECAST_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QObject>

class WeatherForecastModel;

class WeatherForecast : public QObject {
  
  Q_OBJECT
  
signals:
  /**
   * Gives pointer to the prepared model.
   * This model has to be deleted.
   */
  void forecastReady(WeatherForecastModel*);
  
public:
  explicit WeatherForecast(QObject* = 0);
  
  void fetchForecast(const QString&, const QString&);
  
  inline static WeatherForecastModel *
  progressModel() {
    return __progressModel;
  }

private slots:
  void __readyRead();
  void __finished();
  
private:
  QNetworkAccessManager __nam;
  
  QByteArray __data;
  
  QNetworkReply* __reply;
  
  static WeatherForecastModel* __progressModel;

};

#endif // WEATHERFORECAST_H
