/*
 * weatherdata.h
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

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QObject>

class WeatherData : public QObject {
  Q_OBJECT

public:
  /**
   * Each value of this enum corresponds to one icon that
   * is being displayed in the widget.
   */
  enum Condition {
    Clear,
    CloudsFew,
    CloudsMedium,
    Overcast,
    Drizzle,
    RainLight,
    RainMedium,
    RainAndWind,
    SnowLight,
    SnowMedium,
    SnowHeavy,
    Foggy,
    Breezy,
    Windy,
  };
  Q_ENUMS(WeatherForecastModel::Condition);
  
  struct Temperature {
    int celsius;
    int fahrenheit;
  };
  
  explicit WeatherData(QObject* = nullptr);
  
  /**
   * Constructs the WeatherData object from data given directly.
   * @param dayOfWeek Day of week.
   * @param condition Condition during the given day.
   * @param description Description text (e.g. "Light rain in the morning").
   */
  explicit WeatherData(QString, Condition, QString = QString(), QObject* = nullptr);
  
  void setDayOfWeek(const QString&);
  void setCondition(Condition);
  void setDescription(const QString&);
  void setLow(const Temperature&);
  void setHigh(const Temperature&);
  
  inline const QString& dayOfWeek() const { return __dayOfWeek; }
  inline Condition condition() const { return __condition; }
  inline const QString& description() const { return __description; }
  inline const Temperature& low() const { return __low; }
  inline const Temperature& high() const { return __high; }
  
private:
  QString __dayOfWeek;
  Condition __condition;
  QString __description;
  Temperature __low;
  Temperature __high;
  
};

#endif // WEATHERDATA_H
