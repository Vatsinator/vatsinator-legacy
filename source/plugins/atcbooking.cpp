/*
 * atcbooking.cpp
 * Copyright (C) 2014  Michal Garapich <michal@garapich.pl>
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

#include "atcbooking.h"

AtcBooking::AtcBooking(QObject* parent): QObject(parent) {}

void
AtcBooking::setCallsign(const QString& callsign)
{
    __callsign = callsign;
}

void AtcBooking::setRealName(const QString& realName)
{
    __realName = realName;
}

void AtcBooking::setTrainingSession(bool trainingSession)
{
    __trainingSession = trainingSession;
}

void AtcBooking::setBookedDay(const QDate& bookedDay)
{
    __bookedDay = bookedDay;
}

void AtcBooking::setTimeFrom(const QTime& timeFrom)
{
    __timeFrom = timeFrom;
}

void AtcBooking::setTimeTo(const QTime& timeTo)
{
    __timeTo = timeTo;
}
