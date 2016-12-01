/*
 * airline.cpp
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

#include "airline.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class AirlineData : public QSharedData {
public:
    AirlineData() = default;
    explicit AirlineData(const QString& icao) : icao(icao) {}
    
    QString icao;
    QString iata;
    QString name;
};

Airline::Airline() :
    d(new AirlineData) {}

Airline::Airline(const QString& icao) :
    d(new AirlineData(icao)) {}

Airline::Airline(const Airline& other) :
    d(other.d) {}

Airline::~Airline() {}

Airline& Airline::operator=(const Airline& other)
{
    d = other.d;
    return *this;
}

QString Airline::icao() const
{
    return d->icao;
}

void Airline::setIcao(const QString& icao)
{
    d->icao = icao;
}

QString Airline::iata() const
{
    return d->iata;
}

void Airline::setIata(const QString& iata)
{
    d->iata = iata;
}

QString Airline::name() const
{
    return d->name;
}

void Airline::setName(const QString& name)
{
    d->name = name;
}

bool operator==(const Airline& a, const Airline& b)
{
    return a.icao() == b.icao() && a.iata() == b.iata() &&
        a.name() == b.name();
}

bool operator!=(const Airline& a, const Airline& b)
{
    return a.icao() != b.icao() || a.iata() != b.iata() ||
        a.name() != b.name();
}

}} /* namespace Vatsinator::Core */
