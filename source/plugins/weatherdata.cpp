/*
 * weatherdata.cpp
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

#include "weatherdata.h"

WeatherData::WeatherData(QObject* parent) : QObject(parent) {}

WeatherData::WeatherData(QString dayOfWeek, WeatherData::Condition condition,
                         QString description, QObject* parent) :
    QObject(parent),
    __dayOfWeek(dayOfWeek),
    __condition(condition),
    __description(description) {}

void
WeatherData::setDayOfWeek(const QString& dayOfWeek) {
  __dayOfWeek = dayOfWeek;
}

void
WeatherData::setCondition(WeatherData::Condition condition) {
  __condition = condition;
}

void
WeatherData::setDescription(const QString& description) {
  __description = description;
}

void
WeatherData::setLow(const WeatherData::Temperature& low) {
  __low = low;
}

void
WeatherData::setHigh(const WeatherData::Temperature& high) {
  __high = high;
}
