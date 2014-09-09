/*
 * ultimateweatherforecasts.cpp
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

#include <QtCore>
#include <QtNetwork>

#include "plugins/weatherdata.h"

#include "ultimateweatherforecasts.h"

static const QString ApiUrl = 
  QStringLiteral("https://george-vustrey-weather.p.mashape.com/api.php?location=%1");
static const QString ApiKey =
  QStringLiteral("NXOZbJJxNTmsh1dHGW16LoflT5N1p1jVGIbjsn4Z5b4xFu6VZT");

namespace {
  
  struct Regex2Cond {
    QRegExp regex;
    WeatherData::Condition condition;
  };
  
  QList<Regex2Cond> regexConditions() {
    QList<Regex2Cond> rc({
      { QRegExp("^Clear"), WeatherData::Clear },
      { QRegExp("^Partly cloudy"), WeatherData::CloudsFew },
      { QRegExp("^Mostly cloud"), WeatherData::CloudsMedium },
      { QRegExp("^Overcast"), WeatherData::Overcast },
      { QRegExp("^Drizzle (?!.*breezy)"), WeatherData::Breezy },
      { QRegExp("^Light rain"), WeatherData::RainLight },
      { QRegExp("^Breezy(.+)light rain"), WeatherData::RainLight },
      { QRegExp("^Light snow"), WeatherData::SnowLight },
      { QRegExp("^Snow"), WeatherData::SnowMedium },
      { QRegExp("^Heavy snow"), WeatherData::SnowHeavy },
      { QRegExp("^Foggy"), WeatherData::Foggy },
      { QRegExp("^Breezy"), WeatherData::Breezy },
      { QRegExp("^Drizzle(.+)breezy"), WeatherData::Breezy },
      { QRegExp("^Windy"), WeatherData::Windy },
      { QRegExp("^Dangerously windy"), WeatherData::Windy },
      { QRegExp("^Rain (?!.*windy)"), WeatherData::RainMedium },
      { QRegExp("^Rain and(.+)windy"), WeatherData::RainAndWind }
    });
    
    return qMove(rc);
  }
  
  WeatherData::Condition parseCondition(const QString& str) {
    auto c = regexConditions();
    
    for (auto k: c) {
      if (k.regex.indexIn(str) != -1) {
        return k.condition;
      }
    }
    
    return WeatherData::Clear;
  }
  
  QString getLongDayName(const QString& name) {
    if (name == "Mon")
      return QObject::tr("Monday");
    if (name == "Tue")
      return QObject::tr("Tuesday");
    if (name == "Wed")
      return QObject::tr("Wednesday");
    if (name == "Thu")
      return QObject::tr("Thursday");
    if (name == "Fri")
      return QObject::tr("Friday");
    if (name == "Sat")
      return QObject::tr("Saturday");
    if (name == "Sun")
      return QObject::tr("Sunday");
    return "";
  }
}

UltimateWeatherForecasts::UltimateWeatherForecasts() :
    __reply(nullptr) {}

WeatherForecastReply* UltimateWeatherForecasts::fetch(
    WeatherForecastRequest* request) {
  
  WeatherForecastReply* reply = new WeatherForecastReply(request, this);
  __pendingForecasts.enqueue(reply);
  __newRequest();
  return reply;
}

void
UltimateWeatherForecasts::__newRequest() {
  if (__reply || __pendingForecasts.isEmpty())
    return;
  
  const WeatherForecastReply* r = __pendingForecasts.head();
  QString location = QString("%1, %2").arg(
    r->request()->country(),
    r->request()->city()
  );
  QString url = QString(ApiUrl).arg(location);
  QNetworkRequest request(url);
  request.setRawHeader("X-Mashape-Authorization", ApiKey.toUtf8());
  
  __reply = __nam.get(request);
  connect(__reply, SIGNAL(finished()), this, SLOT(__finished()));
  connect(__reply, SIGNAL(readyRead()), this, SLOT(__readyRead()));
}

void
UltimateWeatherForecasts::__finishRequest() {
  QJsonParseError error;
  QJsonDocument document = QJsonDocument::fromJson(__data, &error);
  WeatherForecastReply* r = __pendingForecasts.dequeue();
  
  if (error.error == QJsonParseError::NoError) {
    QJsonArray array = document.array();
    int i = 0;
    for (QJsonValueRef w: array) {
      i += 1;
      
      /* https://www.mashape.com/george-vustrey/ultimate-weather-forecasts/support/9 */
      if (i == 1)
        continue;
      
      QJsonObject map = w.toObject();
      WeatherData* data = new WeatherData(
        getLongDayName(map["day_of_week"].toString()),
        parseCondition(map["condition"].toString()),
        map["condition"].toString()
      );
      
      WeatherData::Temperature high{
        map["high_celsius"].toString().toInt(),
        static_cast<int>(map["high"].toString().toDouble())
      };
      data->setHigh(high);
      
      WeatherData::Temperature low{
        map["low_celsius"].toString().toInt(),
        static_cast<int>(map["low"].toString().toDouble())
      };
      data->setLow(low);
      
      r->appendWeatherData(data);
    }
  } else {
    r->setError(WeatherForecastReply::NetworkError);
  }
  
  __forecastReplies << r;
  r->setFinished(true);
  
  __newRequest();
}

void UltimateWeatherForecasts::__readyRead() {
  __data.append(__reply->readAll());
}

void UltimateWeatherForecasts::__finished() {
  switch (__reply->error()) {
    case QNetworkReply::NoError:
      __reply->deleteLater();
      __reply = nullptr;
      __finishRequest();
      __data.clear();
      break;
      
    case QNetworkReply::TimeoutError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::ContentReSendError: {
      QNetworkRequest request = __reply->request();
      
      __reply->deleteLater();
      __reply = __nam.get(request);
      connect(__reply, SIGNAL(finished()), this, SLOT(__finished()));
      connect(__reply, SIGNAL(readyRead()), this, SLOT(__readyRead()));
      __data.clear();
      break;
    }
    
    default:
      __newRequest();
  }
}
