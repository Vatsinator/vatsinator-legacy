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
#include <QtCore>

namespace Vatsinator { namespace Core {

class AirportData : public QSharedData {
public:
    AirportData() = default;
    AirportData(const QString& icao) : icao(icao), altitude(0) {}
    
    QString icao;
    QString iata;
    QString city;
    QString country;
    QString name;
    LonLat position;
    int altitude;
};


Airport::Airport():
    d(new AirportData) {}

Airport::Airport(const QString& icao) :
    d(new AirportData(icao)) {} // TODO Validate ICAO code

Airport::Airport(const Airport& other) :
    d(other.d) {}

Airport::~Airport() {}

Airport& Airport::operator=(const Airport& other)
{
    d = other.d;
    return *this;
}

QString Airport::icao() const
{
    return d->icao;
}

QString Airport::iata() const
{
    return d->iata;
}

void Airport::setIata(const QString& iata)
{
    d->iata = iata;
}

QString Airport::city() const
{
    return d->city;
}

void Airport::setCity(const QString& city)
{
    d->city = city;
}

QString Airport::country() const
{
    return d->country;
}

void Airport::setCountry(const QString& country)
{
    d->country = country;
}

QString Airport::name() const
{
    return d->name;
}

void Airport::setName(const QString& name)
{
    d->name = name;
}

LonLat Airport::position() const
{
    return d->position;
}

void Airport::setPosition(const LonLat& position)
{
    d->position = position;
}

int Airport::altitude() const
{
    return d->altitude;
}

void Airport::setAltitude(int altitude)
{
    d->altitude = altitude;
}

bool Airport::isValid() const
{
    return !d->icao.isEmpty();
}

}} /* namespace Vatsinator::Core */
