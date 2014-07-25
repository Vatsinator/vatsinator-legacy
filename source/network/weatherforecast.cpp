/*
 * weatherforecast.cpp
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

#include <QtNetwork>

#include "network/models/weatherforecastmodel.h"
#include "config.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "weatherforecast.h"

static QString WeatherForecastApiUrl =
  QString(NetConfig::Vatsinator::apiUrl()) + QString("weatherforecast.php?location=%1");

WeatherForecast::WeatherForecast(QObject* _parent) :
    QObject(_parent),
    __reply(nullptr) {}

void
WeatherForecast::fetchForecast(const QString& _city, const QString& _country) {
  if (__reply) {
    __reply->abort();
    __reply->deleteLater();
  }
  __data.clear();
  
  QString location = _city % ", " % _country;
  
  QString url = QString(WeatherForecastApiUrl).arg(location);
  QNetworkRequest request(url);
  request.setRawHeader("User-Agent", "Vatsinator/" VATSINATOR_VERSION);
  
  VatsinatorApplication::log("WeatherForecast: request: %s", qPrintable(request.url().toString()));
  
  __reply = __nam.get(request);
  connect(__reply, SIGNAL(finished()), this, SLOT(__finished()));
  connect(__reply, SIGNAL(readyRead()), this, SLOT(__readyRead()));
}

void
WeatherForecast::__readyRead() {
  __data.append(__reply->readAll());
}

void
WeatherForecast::__finished() {
  switch (__reply->error()) {
    case QNetworkReply::NoError: {
      __reply->deleteLater();
      __reply = nullptr;
      
      WeatherForecastModel* model = new WeatherForecastModel(__data);
      emit forecastReady(model);
      
      break;
    }
    case QNetworkReply::TimeoutError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::ContentReSendError: {
      QNetworkRequest request = __reply->request();
      
      __reply->deleteLater();
      __reply = __nam.get(request);
      connect(__reply, SIGNAL(finished()), this, SLOT(__finished()));
      connect(__reply, SIGNAL(readyRead()), this, SLOT(__readyRead()));
      
      break;
    }
    default:
      break;
  }
}

