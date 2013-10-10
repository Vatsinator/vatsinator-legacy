/*
 * weatherforecastmodel.h
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

#ifndef WEATHERFORECASTMODEL_H
#define WEATHERFORECASTMODEL_H

#include <QAbstractTableModel>
#include <QByteArray>
#include <QVector>
#include <QMap>

class WeatherForecastModel : public QAbstractTableModel {
  
  /**
   * This is model for the weather forecast.
   */
  
  Q_OBJECT
  
public:
  
  explicit WeatherForecastModel(const QByteArray&, QObject* = 0);
  explicit WeatherForecastModel(QObject* = 0);
  
  int rowCount(const QModelIndex& = QModelIndex()) const;
  int columnCount(const QModelIndex& = QModelIndex()) const;
  QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
  
private:
  
  enum DataStatus {
    Progress,
    Error,
    Fetched
  };
  
  struct ForecastForDay {
    QString day;
    float   high;
    float   low;
    QString condition;
  };
  
  void __parseJson(const QByteArray&);
  
  static QMap<QString, QString> __iconsMapFilled();
  
  QVector<ForecastForDay> __data;
  DataStatus __status;
  
  /* The Ultimate Weather Forecasts returns condition as a string.
   * As for september of 2013, returning condition as a number is one of
   * the requests in their issue tracker.
   * So, as for now, we have to put all the strings that the API can
   * return in the map and assign pixmaps to it.
   */
  static QMap<QString, QString> __iconsMap;
  
};

#endif // WEATHERFORECASTMODEL_H
