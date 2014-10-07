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

/**
 * The WeatherData class is used to represent weather for a single day of week.
 */
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
  Q_ENUMS(WeatherData::Condition)
  
  struct Temperature {
    int celsius;
    int fahrenheit;
  };
  
private:
  /**
   * Day of week.
   * The value should be a log day name (e.g. "Monday", "Tuesday" instead
   * of "Mon" and "Tue") and translated.
   */
  Q_PROPERTY(QString dayOfWeek READ dayOfWeek WRITE setDayOfWeek)
  
  /**
   * Condition.
   * Depending on the condition, different icon will be displayed in the widget.
   */
  Q_PROPERTY(Condition condition READ condition WRITE setCondition)
  
  /**
   * The description provides more specific information about the forecast.
   */
  Q_PROPERTY(QString description READ description WRITE setDescription)
  
  /**
   * The lowest expected temperature throughout the day.
   */
  Q_PROPERTY(Temperature low READ low WRITE setLow)
  
  /**
   * The highest expected temperature throughout the day.
   */
  Q_PROPERTY(Temperature high READ high WRITE setHigh)

public:
  /**
   * The default constrcutor passes _parent_ to QObject.
   */
  explicit WeatherData(QObject* parent = nullptr);
  
  /**
   * Constructs the WeatherData object from data given directly.
   * 
   * \param dayOfWeek Day of week.
   * \param condition Condition during the given day.
   * \param description Description text (e.g. "Light rain in the morning").
   */
  explicit WeatherData(QString dayOfWeek, Condition condition,
                       QString description = QString(), QObject* parent = nullptr);
  
  void setDayOfWeek(const QString& dayOfWeek);
  void setCondition(Condition condition);
  void setDescription(const QString& description);
  void setLow(const Temperature& low);
  void setHigh(const Temperature& high);
  
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

Q_DECLARE_METATYPE(WeatherData::Temperature)

#endif // WEATHERDATA_H
