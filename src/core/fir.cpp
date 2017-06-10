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

Fir::Fir(const QString &icao, QObject *parent) :
    ClientList(parent),
    m_icao(icao) {}

void Fir::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(m_name);
    }
}

void Fir::setCountry(const QString& country)
{
    if (m_country != country) {
        m_country = country;
        emit countryChanged(country);
    }
}

void Fir::setOceanic(bool oceanic)
{
    if (m_oceanic != oceanic) {
        m_oceanic = oceanic;
        emit oceanicChanged(m_oceanic);
    }
}

void Fir::setLabelPosition(const LonLat& labelPosition)
{
    if (m_labelPosition != labelPosition) {
        m_labelPosition = labelPosition;
        emit labelPositionChanged(m_labelPosition);
    }
}

void Fir::setBoundaries(const QList<Fir::Boundaries>& boundaries)
{
    m_boundaries = boundaries;
    emit boundariesChanged(m_boundaries);
}

}} /* namespace Vatsinator::Core */
