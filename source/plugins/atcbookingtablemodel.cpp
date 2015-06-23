/*
 * atcbookingtablemodel.cpp
 * Copyright (C) 2014  Michal Garapich <michal@garapich.pl>
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

#include <QtGui>

#include "plugins/atcbooking.h"

#include "atcbookingtablemodel.h"

AtcBookingTableModel::AtcBookingTableModel(QObject* parent):
    QAbstractTableModel(parent)
{

}

void
AtcBookingTableModel::add(AtcBooking* booking)
{
    __bookings << booking;
    booking->setParent(this);
}

int
AtcBookingTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    /**
     * 0 - callsign
     * 1 - realName
     * 2 - date booked
     * 3 - hours booked
     */
    return 4;
}

int
AtcBookingTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return __bookings.size();
}

QVariant
AtcBookingTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount())
        return QVariant();
        
    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
            
        case Qt::ForegroundRole:
            if (__bookings.at(index.row())->isTrainingSession())
                return QBrush(Qt::blue);
            else
                return QVariant();
                
        case Qt::DisplayRole:
            switch (index.column()) {
                case Callsign:
                    return __bookings.at(index.row())->callsign();
                    
                case Name:
                    return __bookings.at(index.row())->realName();
                    
                case Date:
                    return __bookings.at(index.row())->bookedDay().toString("dd MMM yyyy");
                    
                case Hours:
                    return QString("%1 - %2").arg(
                               __bookings.at(index.row())->timeFrom().toString("hh:mm"),
                               __bookings.at(index.row())->timeTo().toString("hh:mm"));
                               
                default:
                    return QVariant();
            }
            
        default:
            return QVariant();
    }
}

QVariant
AtcBookingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= columnCount() || orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
        
    switch (section) {
        case Callsign:
            return tr("Callsign");
            
        case Name:
            return tr("Name");
            
        case Date:
            return tr("Date");
            
        case Hours:
            return tr("Hours");
            
        default:
            return QVariant();
    }
}

void
AtcBookingTableModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();
    
    switch(column) {
        case Callsign:
            if (order == Qt::AscendingOrder)
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return a->callsign() < b->callsign();
            });
            else
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return a->callsign() > b->callsign();
            });
            break;
            
        case Name:
            if (order == Qt::AscendingOrder)
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return a->realName() < b->realName();
            });
            else
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return a->realName() > b->realName();
            });
            break;
            
        case Date:
            if (order == Qt::AscendingOrder)
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return a->bookedDay() < b->bookedDay();
            });
            else
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return a->bookedDay() > b->bookedDay();
            });
            break;
            
        case Hours:
            if (order == Qt::AscendingOrder)
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return QDateTime(a->bookedDay(), a->timeFrom()) <
                       QDateTime(b->bookedDay(), b->timeFrom());
            });
            else
                std::sort(__bookings.begin(), __bookings.end(), [] (const AtcBooking * a, const AtcBooking * b) {
                return QDateTime(a->bookedDay(), a->timeFrom()) <
                       QDateTime(b->bookedDay(), b->timeFrom());
            });
            break;
    }
    
    endResetModel();
}

