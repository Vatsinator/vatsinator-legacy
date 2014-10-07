/*
 * weatherforecastwidget.h
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

#ifndef WEATHERFORECASTWIDGET_H
#define WEATHERFORECASTWIDGET_H

#include <QList>
#include "ui/widgets/delayedwidget.h"

class WeatherData;

/**
 * The WeatherForecastWidget class is a special widget designed particulary
 * for rendering the weather forecast. It uses WeatherData objects to read
 * forecast data. This widget supports days of week, weather forecast icons,
 * temperatures (low and high) and condition descriptions display.
 */
class WeatherForecastWidget : public DelayedWidget {
  Q_OBJECT
  
  /**
   * This property holds the message to be displayed instead of weather forecast.
   * This can be used i.e. to display the information that the forecast is
   * currently being downloaded from the internet.
   * As long as the message is not empty, the weather forecast will not
   * be drawn. To show the weather back, simply set value to the empty string.
   */
  Q_PROPERTY(QString message READ message WRITE setMessage)
  
  /**
   * The maxItemCount property holds maximum weather forecast days
   * that will be rendered. This is automatically set to data vector size,
   * but can be easily overriden.
   */
  Q_PROPERTY(int maxItemCount READ maxItemCount WRITE setMaxItemCount)
  
public:
  /**
   * The default constructor passes _parent_ to DelayedWidget.
   */
  WeatherForecastWidget(QWidget* parent = nullptr);
  
  /**
   * Sets weather forecast data.
   * Note that the data pointers must remain valid as long as the widget is
   * visible.
   * 
   * \sa setData().
   */
  void setData(const QList<WeatherData*> data);
  
  /**
   * Sets temperature units to Celsius.
   * Celsius is the default scale.
   * \sa setFahrenheit().
   */
  void setCelsius();
  
  /**
   * Sets temperature units to Fahrenheit.
   * \sa setCelsius().
   */
  void setFahrenheit();
  
  void setMessage(const QString& message);
  inline const QString& message() const { return __message; }
  void setMaxItemCount(int count);
  inline int maxItemCount() const { return __maxItemCount; }
  
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;
  
protected:
  void paintEvent(QPaintEvent* event) override;
  
private:
  QList<WeatherData*> __data;
  QString __message;
  int __maxItemCount;
  bool __celsius;

};

#endif // WEATHERFORECASTWIDGET_H
