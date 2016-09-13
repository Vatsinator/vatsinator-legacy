/*
 * clientlist.cpp
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

#include "clientlist.h"

namespace Vatsinator { namespace Core {

ClientList::ClientList(QObject* parent) : QObject(parent) {}

void ClientList::add(const Client* client)
{
    if (!contains(client)) {
        m_clients.insert(client);
        connect(client, &QObject::destroyed, this, &ClientList::removeClient);
        emit clientAdded(client);
        emit clientCountChanged();
    }
}

bool ClientList::contains(const Client* client) const
{
    return m_clients.contains(client);
}

void ClientList::remove(const Client* client)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [client](auto it) {
        return it == client;
    });
    
    if (it != m_clients.end()) {
        disconnect(*it);
        m_clients.erase(it);
        emit clientCountChanged();
    }
}

int ClientList::count() const
{
    return m_clients.size();
}

void ClientList::removeClient(QObject* obj)
{
    remove(reinterpret_cast<const Client*>(obj));
}

}} /* namespace Vatsinator::Core */
