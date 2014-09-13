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

#include <QVector>
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
  Q_PROPERTY(QString message READ message WRITE setMessage)
  
public:
  WeatherForecastWidget(QWidget* = nullptr, Qt::WindowFlags = 0);
  
  /**
   * Sets weather forecast data.
   * Note that the data pointers must remain valid as long as the widget is
   * visible.
   * @sa setData().
   */
  void setData(const QVector<WeatherData*>);
  
  /**
   * Sets the message to be displayed instead of weather forecast. This can
   * be used i.e. to display the information that the forecast is currently
   * being downloaded from the internet.
   * As long as the message is not empty, the weather forecast will not
   * be drawn. To unset the message, simply call setMessage("").
   */
  void setMessage(const QString&);
  
  /**
   * Sets max item count to the given value.
   * @sa maxItemCount();
   */
  void setMaxItemCount(int);
  
  /**
   * Sets temperature units to Celsius.
   * Celsius is the default scale.
   * @sa setFahrenheit().
   */
  void setCelsius();
  
  /**
   * Sets temperature units to Fahrenheit.
   * @sa setCelsius().
   */
  void setFahrenheit();
  
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;
  
  /**
   * Gets the message.
   * @sa setMessage().
   */
  inline const QString& message() const { return __message; }
  
  /**
   * maxItemCount is the property that holds maximum weather forecast days
   * that will be rendered. This is automatically set to data vector size,
   * but can be easily overriden.
   * @sa setMaxItemCount().
   */
  inline int maxItemCount() const { return __maxItemCount; }
  
protected:
  void paintEvent(QPaintEvent*) override;
  
private:
  QVector<WeatherData*> __data;
  QString __message;
  int __maxItemCount;
  bool __celsius;

};

#endif // WEATHERFORECASTWIDGET_H
