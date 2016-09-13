/*
 * vatsimdatadocument.cpp
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

#include "vatsimdatadocument.h"
#include <QSharedData>

namespace Vatsinator { namespace Core {

class VatsimDataDocumentData : public QSharedData {
public:
    int connectedClients;
    QDateTime update;
    int reload;
    QList<ClientData> clients;
    QList<ClientData> prefiled;
};

VatsimDataDocument::VatsimDataDocument() :
    d(new VatsimDataDocumentData) {}

VatsimDataDocument::VatsimDataDocument(const VatsimDataDocument& other) :
    d(other.d) {}

VatsimDataDocument::~VatsimDataDocument() {}

VatsimDataDocument& VatsimDataDocument::operator=(const VatsimDataDocument& other)
{
    d = other.d;
    return *this;
}

int VatsimDataDocument::connectedClients() const
{
    return d->connectedClients;
}

void VatsimDataDocument::setConnectedClients(int connectedClients)
{
    d->connectedClients = connectedClients;
}

QDateTime VatsimDataDocument::update() const
{
    return d->update;
}

void VatsimDataDocument::setUpdate(const QDateTime& update)
{
    d->update = update;
}

int VatsimDataDocument::reload() const
{
    return d->reload;
}

void VatsimDataDocument::setReload(int reload)
{
    d->reload = reload;
}

const QList<ClientData>& VatsimDataDocument::clients() const
{
    return d->clients;
}

void VatsimDataDocument::addClient(ClientData client)
{
    d->clients << client;
}

const QList<ClientData>& VatsimDataDocument::prefiled() const
{
    return d->prefiled;
}

void VatsimDataDocument::addPrefiled(ClientData client)
{
    d->prefiled << client;
}

}} /* namespace Vatsinator::Core */
