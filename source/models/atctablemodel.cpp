/*
 * atctablemodel.cpp
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#include <algorithm>
#include <QtCore>

#include "models/roles.h"
#include "vatsimdata/controller.h"

#include "atctablemodel.h"

AtcTableModel::AtcTableModel(QObject* parent): QAbstractTableModel(parent) {}

void
AtcTableModel::add(const Controller* atc)
{
    Q_ASSERT(atc);
    
    if (!__staff.contains(atc)) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        __staff << atc;
        endInsertRows();
        
        connect(atc, &Controller::invalid, this, &AtcTableModel::__autoRemove);
    }
}

void
AtcTableModel::remove(const Controller* atc)
{
    Q_ASSERT(atc);
    
    for (int i = 0; i < __staff.size(); ++i) {
        if (__staff[i] == atc) {
            beginRemoveRows(QModelIndex(), i, i);
            __staff.removeAt(i);
            endRemoveRows();
            
            return;
        }
    }
    
    Q_UNREACHABLE();
}

bool
AtcTableModel::contains(const Controller* atc) const
{
    return __staff.contains(atc);
}

int
AtcTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return __staff.size();
}

int
AtcTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    /* 0 - callsign
     * 1 - name
     * 2 - frequency
     */
    return 3;
}

QVariant
AtcTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount())
        return QVariant();
        
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
            
        case Qt::ToolTipRole:
            return Controller::ratings[__staff.at(index.row())->rating()];
            
        case Qt::DisplayRole:
        
            switch (index.column()) {
                case Callsign:
                    return __staff.at(index.row())->callsign();
                    
                case Name:
                    return __staff.at(index.row())->realName();
                    
                case Frequency:
                    return __staff.at(index.row())->frequency();
                    
                default:
                    return QVariant();
            }
            
        case InstancePointerRole: {
            Controller* c = const_cast<Controller*>(__staff.at(index.row()));
            return QVariant::fromValue(reinterpret_cast<void*>(c));
        }
        
        case CallsignRole:
            return __staff.at(index.row())->callsign();
            
        case RealNameRole:
            return __staff.at(index.row())->realName();
            
        case IcaoRole:
            return __staff.at(index.row())->icao();
            
        case FrequencyRole:
            return __staff.at(index.row())->frequency();
            
        case DescriptionRole:
            return __staff.at(index.row())->description();
        
        default:
            return QVariant();
    }
}

QVariant
AtcTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= columnCount() || orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
        
    switch (section) {
        case 0:
            return tr("Callsign");
            
        case 1:
            //: Name of the ATC (i.e. John Smith EGLL)
            return tr("Name");
            
        case 2:
            return tr("Frequency");
            
        default:
            return QVariant();
    }
}

void
AtcTableModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();
    
    switch (column) {
        case Callsign:
            if (order == Qt::AscendingOrder) {
                std::sort(__staff.begin(), __staff.end(), [] (const Controller * a, const Controller * b) {
                    return a->callsign() < b->callsign();
                });
            } else {
                std::sort(__staff.begin(), __staff.end(), [] (const Controller * a, const Controller * b) {
                    return a->callsign() > b->callsign();
                });
            }
            
            break;
            
        case Name:
            if (order == Qt::AscendingOrder) {
                std::sort(__staff.begin(), __staff.end(), [] (const Controller * a, const Controller * b) {
                    return a->realName() < b->realName();
                });
            } else {
                std::sort(__staff.begin(), __staff.end(), [] (const Controller * a, const Controller * b) {
                    return a->realName() > b->realName();
                });
            }
            
            break;
    }
    
    endResetModel();
}

QHash<int, QByteArray>
AtcTableModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    
    roles[CallsignRole] = "callsign";
    roles[RealNameRole] = "realName";
    roles[IcaoRole] = "icao";
    roles[FrequencyRole] = "frequency";
    roles[DescriptionRole] = "description";
    
    return roles;
}

void
AtcTableModel::__autoRemove()
{
    const Controller* c = qobject_cast<const Controller*>(sender());
    Q_ASSERT(c);
    remove(c);
}
