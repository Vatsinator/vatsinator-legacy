/*
 * atcbookinglistmodel.cpp
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

#include "atcbookinglistmodel.h"
#include "misc/roles.h"
#include <QtGui>

namespace Vatsinator { namespace Misc {

AtcBookingListModel::AtcBookingListModel(QObject* parent) : QAbstractListModel(parent) {}

AtcBookingListModel::AtcBookingListModel(const QList<AtcBooking>& bookings, QObject* parent) :
    QAbstractListModel(parent),
    m_bookings(bookings) {}

AtcBookingListModel::~AtcBookingListModel() {}

int AtcBookingListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_bookings.count();
}

int AtcBookingListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant AtcBookingListModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
            
        case Qt::ForegroundRole:
            if (m_bookings.at(index.row()).isTrainingSession())
                return QBrush(Qt::blue);
            else
                return QVariant();
                
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0:
                    return m_bookings.at(index.row()).callsign();
                    
                case 1:
                    return m_bookings.at(index.row()).realName();
                    
                case 2:
                    return m_bookings.at(index.row()).bookedDay().toString("dd MMM yyyy");
                    
                case 3:
                    return QString("%1 - %2").arg(
                               m_bookings.at(index.row()).timeFrom().toString("hh:mm"),
                               m_bookings.at(index.row()).timeTo().toString("hh:mm"));
                    
                default:
                    return QVariant();
            }
            
        case Qt::ToolTipRole:
            if (m_bookings.at(index.row()).isTrainingSession())
                return tr("Training session");
            else
                return QVariant();
            
        default:
            return QVariant();
    }
}

QVariant AtcBookingListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Callsign");
            case 1: return tr("Name");
            case 2: return tr("Date");
            case 3: return tr("Hours");
            default: return QVariant();
        }
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> AtcBookingListModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    
    roles[CallsignRole] = "callsign";
    roles[RealNameRole] = "realName";
    roles[BookedDayRole] = "bookedDay";
    roles[BookedHoursRole] = "bookedHours";
    
    return roles;
}

}} /* namespace Vatsinator::Misc */
