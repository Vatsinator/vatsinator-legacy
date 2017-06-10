/*
    airport.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "airport.h"
#include "pilot.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

Airport::Airport(const QString &icao, QObject *parent) : ClientList(parent), m_icao(icao)
{

}

void Airport::setIcao(const QString &icao)
{
    if (m_icao != icao) {
        m_icao = icao;
        emit icaoChanged(m_icao);
    }
}

void Airport::setIata(const QString &iata)
{
    if (m_iata != iata) {
        m_iata = iata;
        emit iataChanged(m_iata);
    }
}

void Airport::setCity(const QString &city)
{
    if (m_city != city) {
        m_city = city;
        emit cityChanged(m_city);
    }
}

void Airport::setCountry(const QString &country)
{
    if (m_country != country) {
        m_country = country;
        emit countryChanged(m_country);
    }
}

void Airport::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(m_name);
    }
}

void Airport::setPosition(const LonLat &position)
{
    if (m_position != position) {
        m_position = position;
        emit positionChanged(m_position);
    }
}

void Airport::setAltitude(int altitude)
{
    if (m_altitude != altitude) {
        m_altitude = altitude;
        emit altitudeChanged(m_altitude);
    }
}

QList<const Pilot *> Airport::outboundFlights() const
{
    auto flights = clients<Pilot>();
    flights.erase(std::remove_if(flights.begin(), flights.end(), [this](auto it) {
        return it->departure() != this;
    }), flights.end());

    return flights;
}

int Airport::outboundFlightCount() const
{
    auto flights = clients<Pilot>();
    return std::count_if(flights.constBegin(), flights.constEnd(), [this](auto it) {
        return it->departure() == this;
    });
}

QList<const Pilot *> Airport::inboundFlights() const
{
    auto flights = clients<Pilot>();
    flights.erase(std::remove_if(flights.begin(), flights.end(), [this](auto it) {
        return it->destination() != this;
    }), flights.end());

    return flights;
}

int Airport::inboundFlightCount() const
{
    auto flights = clients<Pilot>();
    return std::count_if(flights.constBegin(), flights.constEnd(), [this](auto it) {
        return it->destination() == this;
    });
}

QString Airport::representativeName() const
{
    return city().isEmpty() ? icao() : QStringLiteral("%1 %2").arg(icao(), city());
}

}} /* namespace Vatsinator::Core */
