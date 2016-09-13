/*
 * atclistmodel.cpp
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

#include "atclistmodel.h"
#include "core/atc.h"
#include "misc/roles.h"

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Misc {

AtcListModel::AtcListModel(QObject* parent) : QAbstractListModel(parent) {}

AtcListModel::~AtcListModel() {}

int AtcListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_atcs.count();
}

int AtcListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant AtcListModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        
        case Qt::ToolTipRole:
            return m_atcs.at(index.row())->realName();

        case Qt::DisplayRole:
            switch (index.column()) {
                case 0: return m_atcs.at(index.row())->callsign();
                case 1: return m_atcs.at(index.row())->realName();
                case 2: return m_atcs.at(index.row())->frequency();
                default: return QVariant();
            }

        case CallsignRole:
            return m_atcs.at(index.row())->callsign();
            
        case RealNameRole:
            return m_atcs.at(index.row())->realName();
            
        case FrequencyRole:
            return m_atcs.at(index.row())->frequency();
            
        default:
            return QVariant();
    }
}

QVariant AtcListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Callsign");
            case 1: return tr("Name");
            case 2: return tr("Frequency");
            default: return QVariant();
        }
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> AtcListModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    
    roles[CallsignRole] = "callsign";
    roles[RealNameRole] = "realName";
    roles[FrequencyRole] = "frequency";
    
    return roles;
}

AtcListModel* AtcListModel::staff(const AirportObject* airport, QObject* parent)
{
    return staffImpl(airport, parent);
}

AtcListModel* AtcListModel::staff(const FirObject* fir, QObject* parent)
{
    return staffImpl(fir, parent);
}

void AtcListModel::add(const Atc* atc)
{
    Q_ASSERT(atc);
    
    beginInsertRows(QModelIndex(), m_atcs.count(), m_atcs.count());
    m_atcs << atc;
    endInsertRows();
    
    connect(atc, &QObject::destroyed, this, &AtcListModel::remove, Qt::DirectConnection);
}

AtcListModel* AtcListModel::staffImpl(const ClientList* list, QObject* parent)
{
    AtcListModel* m = new AtcListModel(parent);
    
    if (list) {
        auto adder = [m](const Client* client) {
            if (const Atc* atc = qobject_cast<const Atc*>(client)) {
                m->add(atc);
            }
        };
        
        std::for_each(list->asSet().constBegin(), list->asSet().constEnd(), adder);
        auto conn = connect(list, &ClientList::clientAdded, adder);
        connect(m, &QObject::destroyed, [conn]() { QObject::disconnect(conn); });
    }
    
    return m;
}

void AtcListModel::remove(QObject* obj)
{
    Q_ASSERT(obj);
    
    for (int i = 0; i < m_atcs.size(); ++i) {
        if (m_atcs.at(i) == obj) {
            beginRemoveRows(QModelIndex(), i, i);
            m_atcs.removeAt(i);
            endRemoveRows();
            return;
        }
    }
    
    Q_UNREACHABLE();
}

}} /* namespace Vatsinator::Misc */
