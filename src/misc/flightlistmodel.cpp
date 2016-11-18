/*
 * flightlistmodel.cpp
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#include "flightlistmodel.h"
#include "core/pilot.h"
#include "core/servertracker.h"
#include "misc/roles.h"

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Misc {

FlightListModel::FlightListModel(QObject* parent) : QAbstractListModel(parent) {}

FlightListModel::~FlightListModel() {}

int FlightListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_flights.count();
}

int FlightListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant FlightListModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        
        case Qt::ToolTipRole:
            return m_flights.at(index.row())->realName();

        case Qt::DisplayRole:
            switch (index.column()) {
                case 0: return m_flights.at(index.row())->callsign();
                case 1: return m_flights.at(index.row())->realName();
                case 2: return m_flights.at(index.row())->departure() ? m_flights.at(index.row())->departure()->icao() : QString();
                case 3: return m_flights.at(index.row())->destination() ? m_flights.at(index.row())->destination()->icao() : QString();
                case 4: return m_flights.at(index.row())->aircraft();
                default: return QVariant();
            }

        case CallsignRole:
            return m_flights.at(index.row())->callsign();
            
        case RealNameRole:
            return m_flights.at(index.row())->realName();
            
        case DepartureRole:
            return QVariant::fromValue(m_flights.at(index.row())->departure());
            
        case DestinationRole:
            return QVariant::fromValue(m_flights.at(index.row())->destination());
            
        case AircraftRole:
            return m_flights.at(index.row())->aircraft();
            
        default:
            return QVariant();
    }
}

QVariant FlightListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Callsign");
            case 1: return tr("Name");
            case 2: return tr("From");
            case 3: return tr("To");
            case 4: return tr("Aircraft");
            default: return QVariant();
        }
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> FlightListModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    
    roles[CallsignRole] = "callsign";
    roles[RealNameRole] = "realName";
    roles[DepartureRole] = "departure";
    roles[DestinationRole] = "destination";
    roles[AircraftRole] = "aircraft";
    
    return roles;
}

template<typename Pred>
static FlightListModel* fromClientList(const ClientList* clients, QObject* parent, Pred pred)
{
    FlightListModel* m = new FlightListModel(parent);
    auto adder = [m, pred](const Client* client) {
        if (pred(client))
            m->add(client);
    };
    
    if (clients) {
        std::for_each(clients->asSet().constBegin(), clients->asSet().constEnd(), adder);
        auto conn = QObject::connect(clients, &ClientList::clientAdded, adder);
        QObject::connect(m, &QObject::destroyed, [conn]() { QObject::disconnect(conn); });
    }
    
    return m;
}

FlightListModel* FlightListModel::inbound(const AirportObject* airport, QObject* parent)
{
    auto pred = [airport](const Client* client) {
        if (const Pilot* flight = qobject_cast<const Pilot*>(client))
            return flight->destination() == airport;
        else
            return false;
    };
    
    return fromClientList(airport, parent, pred);
}

FlightListModel* FlightListModel::outbound(const AirportObject* airport, QObject* parent)
{
    auto pred = [airport](const Client* client) {
        if (const Pilot* flight = qobject_cast<const Pilot*>(client))
            return flight->departure() == airport;
        else
            return false;
    };
    
    return fromClientList(airport, parent, pred);
}

FlightListModel* FlightListModel::around(const FirObject* fir, QObject* parent)
{
    auto pred = [](const Client* client) {
        return qobject_cast<const Pilot*>(client);
    };
    
    return fromClientList(fir, parent, pred);
}

FlightListModel*FlightListModel::all(const ServerTracker* server, QObject* parent)
{
    FlightListModel* m = new FlightListModel(parent);

    if (server) {
        auto adder = [m](const Client* c) {
            if (qobject_cast<const Pilot*>(c))
                m->add(c);
        };

        auto clients = server->clients();
        std::for_each(clients.constBegin(), clients.constEnd(), adder);
        auto conn = connect(server, &ServerTracker::clientAdded, adder);
        connect(m, &QObject::destroyed, [conn]() { QObject::disconnect(conn); });
    }

    return m;
}

void FlightListModel::add(const Core::Client* client)
{
    const Pilot* flight = qobject_cast<const Pilot*>(client);
    Q_ASSERT(flight);
    
    beginInsertRows(QModelIndex(), m_flights.count(), m_flights.count());
    m_flights << flight;
    endInsertRows();
    
    connect(flight, &QObject::destroyed, this, &FlightListModel::remove, Qt::DirectConnection);
}

void FlightListModel::remove(QObject* obj)
{
    Q_ASSERT(obj);
    
    for (int i = 0; i < m_flights.size(); ++i) {
        if (m_flights.at(i) == obj) {
            beginRemoveRows(QModelIndex(), i, i);
            m_flights.removeAt(i);
            endRemoveRows();
            return;
        }
    }
    
    Q_UNREACHABLE();
}

}} /* namespace Vatsinator::Misc */
