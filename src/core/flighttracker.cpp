/*
 * flighttracker.cpp
 * Copyright (C) 2017  Michał Garapich <michal@garapich.pl>
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

#include "flighttracker.h"
#include "servertracker.h"
#include "geo.h"

namespace Vatsinator { namespace Core {

FlightTracker::FlightTracker(Pilot* flight, QObject* parent) :
    QObject(parent),
    m_flight(flight)
{
    connect(m_flight, &QObject::destroyed, this, &QObject::deleteLater);
    connect(m_flight, &Pilot::flightPlanChanged, this, &FlightTracker::invalidateAirports);
    connect(m_flight, &Pilot::flightPlanChanged, this, &FlightTracker::update);
    connect(m_flight, &Pilot::positionChanged, this, &FlightTracker::update);
    connect(m_flight, &Pilot::callsignChanged, this, &FlightTracker::updateAirline);

    updateAirline();
    update();
}

std::tuple<Airport*, Airport*> FlightTracker::findAirports()
{
    Airport* dep = nullptr;
    Airport* dest = nullptr;
    if (flight()->flightPlan().departureAirport().isEmpty()) { // departure airport not filled
        Airport* ap = flight()->server()->airports()->nearest(flight()->position());
        qreal d = nmDistance(flight()->position(), ap->position());
        if (d < Pilot::MaximumDistanceFromAirpoirt()) {
            qDebug("Flight %s is at airport %s (nearest one)",
                   qPrintable(flight()->callsign()), qPrintable(ap->icao()));

            dep = ap;
        }
    } else {
        dep = flight()->server()->airports()->findByIcao(flight()->flightPlan().departureAirport());
    }

    if (!flight()->flightPlan().destinationAirport().isEmpty()) {
        dest = flight()->server()->airports()->findByIcao(flight()->flightPlan().destinationAirport());
    }

    return std::make_tuple(dep, dest);
}

void FlightTracker::update()
{
    // departure or destination airports not set, find them
    if (flight()->departure() == nullptr || flight()->destination() == nullptr) {
        Airport *dep, *dest;
        std::tie(dep, dest) = findAirports();

        if (dep)
            flight()->setDeparture(dep);

        if (dest)
            flight()->setDestination(dest);
    }
}

void FlightTracker::updateAirline()
{
//    flight()->setAirline(flight()->server()->airlines()->findByIcao(flight()->callsign().left(3)));
}

void FlightTracker::invalidateAirports()
{
    flight()->setDeparture(nullptr);
    flight()->setDestination(nullptr);
}

}} /* namespace Vatsinator::Core */
