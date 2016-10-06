/*
 * clientlistmodel.cpp
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

#include "clientlistmodel.h"
#include "core/atc.h"
#include "core/pilot.h"
#include "core/servertracker.h"
#include "misc/roles.h"
#include <functional>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Misc {

ClientListModel::ClientListModel(QObject* parent) : QAbstractListModel(parent) {}

ClientListModel::~ClientListModel() {}

void ClientListModel::setSorted(bool sorted)
{
    m_sorted = sorted;
    if (sorted) {
        std::sort(m_clients.begin(), m_clients.end(), [](auto a, auto b) {
            return a->callsign() < b->callsign();
        });
        
        // section by type - ATCs first, flights second
        std::stable_sort(m_clients.begin(), m_clients.end(), [](auto a, auto b) {
            if (qobject_cast<const Atc*>(a)) {
                return qobject_cast<const Atc*>(b) == nullptr;
            } else {
                return false;
            }
        });
    }
}

int ClientListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_clients.count();
}

int ClientListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant ClientListModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        
        case Qt::ToolTipRole:
            return m_clients.at(index.row())->realName();

        case Qt::DisplayRole:
            switch (index.column()) {
                case 0: return m_clients.at(index.row())->callsign();
                case 1: return m_clients.at(index.row())->realName();
                case 2:
                    if (qobject_cast<const Atc*>(m_clients.at(index.row()))) {
                        return QStringLiteral("ATC");
                    } else if (qobject_cast<const Pilot*>(m_clients.at(index.row()))) {
                        return QStringLiteral("PILOT");
                    }
                
                default: return QVariant();
            }

        case CallsignRole:
            return m_clients.at(index.row())->callsign();
            
        case RealNameRole:
            return m_clients.at(index.row())->realName();
            
        case TypeRole:
            if (qobject_cast<const Atc*>(m_clients.at(index.row()))) {
                return QStringLiteral("ATC");
            } else if (qobject_cast<const Pilot*>(m_clients.at(index.row()))) {
                return QStringLiteral("PILOT");
            }
        
        case InstanceRole:
            if (const Atc* atc = qobject_cast<const Atc*>(m_clients.at(index.row())))
                return QVariant::fromValue<Atc*>(const_cast<Atc*>(atc));
            else if (const Pilot* pilot = qobject_cast<const Pilot*>(m_clients.at(index.row())))
                return QVariant::fromValue<Pilot*>(const_cast<Pilot*>(pilot));
            else
                return QVariant();
            
        case FrequencyRole:
            if (const Atc* atc = qobject_cast<const Atc*>(m_clients.at(index.row()))) {
                return atc->frequency();
            } else {
                return QVariant();
            }
        
        case DepartureRole:
            if (const Pilot* p = qobject_cast<const Pilot*>(m_clients.at(index.row())))
                return QVariant::fromValue(p->departure());
            else
                return QVariant();
            
        case DestinationRole:
            if (const Pilot* p = qobject_cast<const Pilot*>(m_clients.at(index.row())))
                return QVariant::fromValue(p->destination());
            else
                return QVariant();
        
        default:
            return QVariant();
    }
}

QVariant ClientListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Callsign");
            case 1: return tr("Name");
            case 2: return QString();
            default: return QVariant();
        }
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> ClientListModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    
    roles[CallsignRole] = "callsign";
    roles[RealNameRole] = "realName";
    roles[TypeRole] = "type";
    roles[InstanceRole] = "instance";
    roles[FrequencyRole] = "frequency";
    roles[DepartureRole] = "departure";
    roles[DestinationRole] = "destination";
    
    return roles;
}

ClientListModel* ClientListModel::clients(const AirportObject* airport, QObject* parent)
{
    ClientListModel* m = new ClientListModel(parent);
    
    if (airport) {
        auto adder = std::bind(&ClientListModel::add, m, std::placeholders::_1);
        
        std::for_each(airport->asSet().constBegin(), airport->asSet().constEnd(), adder);
        auto conn = connect(airport, &AirportObject::clientAdded, adder);
        connect(m, &QObject::destroyed, [conn]() { QObject::disconnect(conn); });
    }
    
    return m;
}

ClientListModel* ClientListModel::clients(const FirObject* fir, QObject* parent)
{
    ClientListModel* m = new ClientListModel(parent);
    
    if (fir) {
        auto adder = std::bind(&ClientListModel::add, m, std::placeholders::_1);
        
        std::for_each(fir->asSet().constBegin(), fir->asSet().constEnd(), adder);
        auto conn = connect(fir, &FirObject::clientAdded, adder);
        connect(m, &QObject::destroyed, [conn]() { QObject::disconnect(conn); });
    }
    
    return m;
}

ClientListModel*ClientListModel::pilots(const ServerTracker* server, QObject* parent)
{
    ClientListModel* m = new ClientListModel(parent);

    if (server) {
        auto adder = [m](Client* c) {
            if (qobject_cast<Pilot*>(c))
                m->add(c);
        };

        auto clients = server->clients();
        std::for_each(clients.constBegin(), clients.constEnd(), adder);
        auto conn = connect(server, &ServerTracker::clientAdded, adder);
        connect(m, &QObject::destroyed, [conn]() { QObject::disconnect(conn); });
    }

    return m;
}

void ClientListModel::add(const Client* client)
{
    Q_ASSERT(client);
    
    beginInsertRows(QModelIndex(), m_clients.count(), m_clients.count());
    m_clients << client;
    endInsertRows();
    
    connect(client, &QObject::destroyed, this, &ClientListModel::remove, Qt::DirectConnection);
}

void ClientListModel::remove(QObject* obj)
{
    Q_ASSERT(obj);
    
    for (int i = 0; i < m_clients.size(); ++i) {
        if (m_clients.at(i) == obj) {
            beginRemoveRows(QModelIndex(), i, i);
            m_clients.removeAt(i);
            endRemoveRows();
            return;
        }
    }
    
    Q_UNREACHABLE();
}

}} /* namespace Vatsinator::Misc */
