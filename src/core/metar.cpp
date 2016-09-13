/*
 * metar.cpp
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#include "metar.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class MetarData : public QSharedData {
public:
    MetarData() = default;
    MetarData(const QString& icao, const QString& metar) :
        icao(icao),
        metar(metar) {}
    
    QString icao;
    QString metar;
};


Metar::Metar() :
    d(new MetarData) {}

Metar::Metar(const QString& metar) :
    d(new MetarData(metar.left(4), metar)) {}

Metar::Metar(const Metar& other) :
    d(other.d) {}

Metar::~Metar() {}

Metar& Metar::operator=(const Metar& other)
{
    d = other.d;
    return *this;
}

QString Metar::icao() const
{
    return d->icao;
}

void Metar::setIcao(const QString& icao)
{
    d->icao = icao;
}

QString Metar::metar() const
{
    return d->metar;
}

void Metar::setMetar(const QString& metar)
{
    d->metar = metar;
}

}} /* namespace Vatsinator::Core */
