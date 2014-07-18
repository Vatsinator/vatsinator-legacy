/*
 * weatherforecastreply.cpp
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

#include "events/requestfinishedevent.h"
#include "plugins/weatherdata.h"
#include "plugins/weatherforecastrequest.h"

#include "weatherforecastreply.h"

WeatherForecastReply::WeatherForecastReply(
  WeatherForecastRequest* _request,
  QObject* _parent) :
      QObject(_parent),
      __request(_request) {}

WeatherForecastReply::~WeatherForecastReply() {
  delete __request;
}

void
WeatherForecastReply::appendWeatherData(WeatherData* _data) {
  _data->setParent(this);
  __data << _data;
}

bool
WeatherForecastReply::event(QEvent* _event) {
  switch (_event->type()) {
    case Event::Type::RequestFinished:
      return requestFinishedEvent(static_cast<RequestFinishedEvent*>(_event));
      
    default:
      return QObject::event(_event);
  }
}

bool
WeatherForecastReply::requestFinishedEvent(RequestFinishedEvent*) {
  emit finished();
}
