/*
 * weatherforecastwidget.cpp
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

#include <QtWidgets>

#include "plugins/weatherdata.h"
#include "vatsinatorapplication.h"

#include "weatherforecastwidget.h"

/* Size of the single forecast card */
constexpr int WeatherWidth = 150;
constexpr int WeatherHeight = 130;

namespace {
  QString iconForCondition(WeatherData::Condition _condition) {
    switch (_condition) {
      case WeatherData::Clear:
        return ":/weather/weather-clear.png";
      case WeatherData::CloudsFew:
        return ":/weather/weather-few-clouds.png";
      case WeatherData::CloudsMedium:
        return ":/weather/weather-clouds.png";
      case WeatherData::Overcast:
        return ":/weather/weather-overcast.png";
      case WeatherData::Drizzle:
        return ":/weather/weather-drizzle.png";
      case WeatherData::RainLight:
        return ":/weather/weather-light-rain.png";
      case WeatherData::RainMedium:
        return ":/weather/weather-rain.png";
      case WeatherData::RainAndWind:
        return ":/weather/weather-rain-windy.png";
      case WeatherData::SnowLight:
        return ":/weather/weather-light-snow.png";
      case WeatherData::SnowMedium:
        return ":/weather/weather-snow.png";
      case WeatherData::SnowHeavy:
        return ":/weather/weather-heavy-snow.png";
      case WeatherData::Foggy:
        return ":/weather/weather-foggy.png";
      case WeatherData::Breezy:
        return ":/weather/weather-breezy.png";
      case WeatherData::Windy:
        return ":/weather/weather-windy.png";
      default:
        return ":/weather/weather-clear.png";
    }
  }
}

WeatherForecastWidget::WeatherForecastWidget(QWidget* parent) :
    DelayedWidget(parent),
    __maxItemCount(0),
    __celsius(true) {}

void
WeatherForecastWidget::setData(const QList<WeatherData*> data) {
  __data = data;
  if (__maxItemCount == 0)
    __maxItemCount = __data.count();
  
  updateGeometry();
  update();
}

void
WeatherForecastWidget::setCelsius() {
  __celsius = true;
  update();
}

void
WeatherForecastWidget::setFahrenheit() {
  __celsius = false;
  update();
}

void
WeatherForecastWidget::setMessage(const QString& message) {
  __message = message;
  updateGeometry();
  update();
}

void
WeatherForecastWidget::setMaxItemCount(int count) {
  __maxItemCount = count;
  updateGeometry();
  update();
}

QSize
WeatherForecastWidget::sizeHint() const {
  if (__data.isEmpty() || !__message.isEmpty())
    return QWidget::sizeHint();
  else
    return QSize(__maxItemCount * WeatherWidth, WeatherHeight);
}

QSize WeatherForecastWidget::minimumSizeHint() const {
  if (__data.isEmpty() || !__message.isEmpty())
    return QWidget::minimumSizeHint();
  else
    return QSize(__maxItemCount * WeatherWidth, WeatherHeight);
}

void
WeatherForecastWidget::paintEvent(QPaintEvent* event) {
  /* Weather icon size */
  constexpr int IconWidth = 50;
  constexpr int IconHeight = 50;
  
  QPainter p(this);
  p.setBackground(QBrush(Qt::white));
  p.fillRect(event->rect(), p.background());
  
  if (status() == Loading) {
    DelayedWidget::paintEvent(event);
    return;
  }
  
  p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
  
  if (!__message.isEmpty()) {
    p.drawText(event->rect(), Qt::AlignCenter | Qt::TextWordWrap, __message);
    return;
  }
  
  if (__data.isEmpty()) {
    return;
  }
  
  int i = 0, iconCount = qMin(__maxItemCount, __data.size());
  int width = QWidget::width() / iconCount;
  int textHeight = QApplication::fontMetrics().height();
  
  for (const WeatherData* d: __data) {
    QRect dayRect(QPoint(i * width, 0), QSize(width, WeatherHeight));
    
    /* Draw day of the week */
    QPen pen(QColor(Qt::black));
    pen.setWidth(3);
    p.setPen(pen);
    
    QRect textRect(dayRect.topLeft(), QSize(width - IconWidth, textHeight));
    textRect.setLeft(textRect.topLeft().x() + 10);
    textRect.moveTop(textRect.topLeft().y() + 10);
    p.drawText(textRect, Qt::AlignLeft, d->dayOfWeek());
    
    pen.setWidth(0);
    p.setPen(pen);
    
    /* Draw temperatures */
    QString degreeSign = QString::fromWCharArray(L"\u00B0");
    int high = __celsius ? d->high().celsius : d->high().fahrenheit;
    int low = __celsius ? d->low().celsius : d->low().fahrenheit;
    
    QRect highRect(QPoint(), QSize(textRect.width(), textHeight));
    highRect.moveTopLeft(textRect.bottomLeft());
    QRect highBoundingRect;
    p.drawText(highRect, Qt::AlignLeft, QString::number(high) + degreeSign, &highBoundingRect);
    
    QRect lowRect(QPoint(), QSize(textRect.width(), textHeight));
    lowRect.moveTopLeft(highBoundingRect.topRight());
    lowRect.moveLeft(lowRect.left() + 5);
    
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    p.drawText(lowRect, Qt::AlignLeft, QString::number(low) + degreeSign);
    
    /* Draw icon */
    QRect iconRect(QPoint(), QSize(IconWidth, IconHeight));
    iconRect.moveTopRight(dayRect.topRight());
    QPoint c = iconRect.center();
    iconRect.setSize(QSize(iconRect.width() - 10, iconRect.height() - 10));
    iconRect.moveCenter(c);
    QPixmap icon(iconForCondition(d->condition()));
    
    p.drawPixmap(iconRect, icon);
    
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    
    /* Draw description */
    QRect descRect(QPoint(), QSize(dayRect.width(), WeatherHeight));
    descRect.setTop(iconRect.bottom());
    descRect.moveRight(iconRect.right());
    c = descRect.center();
    descRect.setWidth(descRect.width() - 30);
    descRect.moveCenter(c);
    if (!d->description().isEmpty()) {
      p.drawText(descRect, Qt::TextWordWrap, d->description());
    }
    
    /* Draw the line that divides days */
    pen.setColor(QColor(233, 233, 233));
    p.setPen(pen);
    p.drawLine(dayRect.topRight(), dayRect.bottomRight());
    
    i += 1;
    
    if (i >= iconCount)
      break;
  }
}
