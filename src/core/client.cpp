/*
    client.cpp
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

#include "client.h"
#include "servertracker.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

Client::Client(quint32 pid, ServerTracker* server) :
    QObject(server),
    m_server(server),
    m_pid(pid) {}

Client::~Client() {}

void Client::setCallsign(const QString& callsign)
{
    if (m_callsign != callsign) {
        m_callsign = callsign;
        emit callsignChanged(m_callsign);
    }
}

void Client::setRealName(const QString& realName)
{
    if (m_realName != realName) {
        m_realName = realName;
        emit realNameChanged(m_realName);
    }
}

void Client::setServerName(const QString& serverName)
{
    if (m_serverName != serverName) {
        m_serverName = serverName;
        emit serverNameChanged(m_serverName);
    }
}

void Client::setOnlineFrom(const QDateTime& onlineFrom)
{
    if (m_onlineFrom != onlineFrom) {
        m_onlineFrom = onlineFrom;
        emit onlineFromChanged(onlineFrom);
    }
}

void Client::setPosition(const LonLat& position)
{
    if (m_position != position) {
        m_position = position;
        emit positionChanged(m_position);
    }
}

}} /* namespace Vatsinator::Core */
