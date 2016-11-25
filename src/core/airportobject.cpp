/*
 * airportobject.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include "airportobject.h"
#include "atc.h"
#include "pilot.h"
#include "servertracker.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

AirportObject::AirportObject(const Airport& airport, ServerTracker* server) :
    ClientList(server),
    m_airport(airport),
    m_known(true),
    m_server(server){}

AirportObject::AirportObject(const QString& icao, ServerTracker* server) :
    ClientList(server),
    m_airport(Airport(icao)),
    m_known(false),
    m_server(server) {}

QString AirportObject::representativeName() const
{
    if (isKnownAirport())
        return QStringLiteral("%1 %2").arg(icao(), city());
    else
        return icao();
}

QList<const Pilot*> AirportObject::outboundFlights() const
{
    auto flights = clients<Pilot>();
    flights.erase(std::remove_if(flights.begin(), flights.end(), [this](auto it) {
        return it->departure() != this;
    }), flights.end());
    
    return flights;
}

QList<const Pilot*> AirportObject::inboundFlights() const
{
    auto flights = clients<Pilot>();
    flights.erase(std::remove_if(flights.begin(), flights.end(), [this](auto it) {
        return it->destination() != this;
    }), flights.end());
    
    return flights;
}

int AirportObject::inboundFlightsCount() const
{
    auto flights = clients<Pilot>();
    return std::count_if(flights.constBegin(), flights.constEnd(), [this](auto it) {
        return it->destination() == this;
    });
}

int AirportObject::outboundFlightsCount() const
{
    auto flights = clients<Pilot>();
    return std::count_if(flights.constBegin(), flights.constEnd(), [this](auto it) {
        return it->departure() == this;
    });
}

}} /* namespace Vatsinator::Core */

static void registerType()
{
    qRegisterMetaType<Vatsinator::Core::AirportObject*>("AirportObject*");
}
Q_COREAPP_STARTUP_FUNCTION(registerType)
