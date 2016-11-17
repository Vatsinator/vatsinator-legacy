/*
 * flightplan.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#include "flightplan.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class FlightPlanData : public QSharedData {
public:
    QString route;
    QString departureAirport;
    QString destinationAirport;

};

FlightPlan::FlightPlan() :
    d(new FlightPlanData) {}

FlightPlan::FlightPlan(const FlightPlan& other) :
    d(other.d) {}

FlightPlan::~FlightPlan() {}

FlightPlan& FlightPlan::operator=(const FlightPlan& other)
{
    d = other.d;
    return *this;
}

bool FlightPlan::operator==(const FlightPlan& other) const
{
    return d == other.d || (
                route() == other.route() &&
                departureAirport() == other.departureAirport() &&
                destinationAirport() == other.destinationAirport());
}

bool FlightPlan::operator!=(const FlightPlan& other) const
{
    return route() != other.route() ||
            departureAirport() != other.departureAirport() ||
            destinationAirport() != other.destinationAirport();
}

bool FlightPlan::isEmpty() const
{
    return d->route.isEmpty();
}

QString FlightPlan::route() const
{
    return d->route;
}

void FlightPlan::setRoute(const QString& route)
{
    d->route = route;
}

QString FlightPlan::departureAirport() const
{
    return d->departureAirport;
}

void FlightPlan::setDepartureAirpoirt(const QString& departureAirpoirt)
{
    d->departureAirport = departureAirpoirt;
}

QString FlightPlan::destinationAirport() const
{
    return d->destinationAirport;
}

void FlightPlan::setDestinationAirport(const QString& destinationAirport)
{
    d->destinationAirport = destinationAirport;
}

}} /* namespace Vatsinator::Core */
