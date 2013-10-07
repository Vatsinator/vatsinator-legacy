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

class WeatherForecast : public QObject {
  
  Q_OBJECT
  
public:
  explicit WeatherForecast(QObject* = 0);
  
  void fetchForecast(const QString&, const QString&);

private slots:
  void __readyRead();
  void __finished();
  
private:
  QNetworkAccessManager __nam;
  
  QString __temp;
  QString __data;
  
  QNetworkReply* __reply;

};

#endif // WEATHERFORECAST_H
