/*
 * fir.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "fir.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class FirData : public QSharedData {
public:
    FirData() = default;
    explicit FirData(const QString& icao) : icao(icao) {}
    
    QString icao;
    QString name;
    QString country;
    bool oceanic = false;
    LonLat labelPosition;
    QList<Fir::Boundaries> boundaries;
};

Fir::Fir() :
    d(new FirData) {}

Fir::Fir(const QString& icao) :
    d(new FirData(icao)) {}

Fir::Fir(const Fir& other) :
    d(other.d) {}

Fir::~Fir() {}

Fir& Fir::operator=(const Fir& other)
{
    d = other.d;
    return *this;
}

QString Fir::icao() const
{
    return d->icao;
}

QString Fir::name() const
{
    return d->name;
}

void Fir::setName(const QString& name)
{
    d->name = name;
}

QString Fir::country() const
{
    return d->country;
}

void Fir::setCountry(const QString& country)
{
    d->country = country;
}

bool Fir::isOceanic() const
{
    return d->oceanic;
}

void Fir::setOceanic(bool oceanic)
{
    d->oceanic = oceanic;
}

LonLat Fir::labelPosition() const
{
    return d->labelPosition;
}

void Fir::setLabelPosition(const LonLat& labelPosition)
{
    d->labelPosition = labelPosition;
}

QList<Fir::Boundaries> Fir::boundaries() const
{
    return d->boundaries;
}

void Fir::setBoundaries(const QList<Fir::Boundaries>& boundaries)
{
    d->boundaries = boundaries;
}

}} /* namespace Vatsinator::Core */
