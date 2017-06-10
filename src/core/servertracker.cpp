/*
 * servertracker.cpp
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

#include "servertracker.h"
#include <QtCore>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Core {

ServerTracker::ServerTracker(QObject *parent) :
    QObject(parent)
{
}

void ServerTracker::setClientModel(ClientModel *model)
{
    if (m_clients != model) {
        m_clients = model;
        emit clientModelChanged(m_clients);
    }
}

void ServerTracker::setAirportModel(AirportModel *airports)
{
    if (m_airports != airports) {
        m_airports = airports;
        emit airportModelChanged(m_airports);
    }
}

void ServerTracker::setFirModel(FirModel *firs)
{
    if (m_firs != firs) {
        m_firs = firs;
        emit firModelChanged(m_firs);
    }
}

void ServerTracker::setMetarModel(MetarListModel *metars)
{
    if (m_metars != metars) {
        m_metars = metars;
        emit metarModelChanged(m_metars);
    }
}

}} /* namespace Vatsinator::Core */
