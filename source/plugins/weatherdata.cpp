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

WeatherData::WeatherData(QObject* _parent) : QObject(_parent) {}

WeatherData::WeatherData(QString _day, WeatherData::Condition _c,
                         QString _description, QObject* _parent) :
    QObject(_parent),
    __dayOfWeek(_day),
    __condition(_c),
    __description(_description) {}

void
WeatherData::setDayOfWeek(const QString& _day) {
  __dayOfWeek = _day;
}

void
WeatherData::setCondition(WeatherData::Condition _c) {
  __condition = _c;
}

void
WeatherData::setDescription(const QString& _description) {
  __description = _description;
}

void
WeatherData::setLow(const WeatherData::Temperature& _low) {
  __low = _low;
}

void
WeatherData::setHigh(const WeatherData::Temperature& _high) {
  __high = _high;
}
