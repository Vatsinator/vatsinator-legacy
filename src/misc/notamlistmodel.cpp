/*
 * notamlistmodel.cpp
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

#include "notamlistmodel.h"
#include "misc/roles.h"
#include <QtGui>

namespace Vatsinator { namespace Misc {

NotamListModel::NotamListModel(QObject* parent) : QAbstractListModel(parent) {}

NotamListModel::NotamListModel(const QList<Notam>& notams, QObject* parent) :
    QAbstractListModel(parent),
    m_notams(notams) {}

NotamListModel::~NotamListModel() {}

int NotamListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_notams.count();
}

int NotamListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant NotamListModel::data(const QModelIndex& index, int role) const
{
    Notam notam = m_notams.at(index.row());
    
    switch (role) {
        case Qt::DisplayRole:
            return QString("%1 %2 %3").arg(
                    notam.icao(),
                    notam.ident(),
                    notam.notam()
                );
            
        case Qt::ToolTipRole: {
            QString to;
            
            if (notam.to().isValid()) {
                to = notam.to().toString("yyyy-MM-dd hh:mm:ss");
                
                if (notam.cflag() == Notam::Est)
                    to += " <strong>EST</strong>";
            } else {
                to = "<strong>PERM</strong>";
            }
                
            if (!notam.diurnal().isEmpty())
                to += "; " + notam.diurnal();
            
            return QString("<p style='white-space:nowrap'>Effective from %1 until %2</p>").arg(
                notam.from().toString("yyyy-MM-dd hh:mm:ss"),
                to
            );
        }
        
        case Qt::BackgroundRole:
            return notam.type() == Notam::Cancellation ? QBrush(QColor(255, 177, 177)) : QVariant();
        
        case IcaoRole:
            return notam.icao();
            
        case IdentRole:
            return notam.ident();
            
        case NotamRole:
            return notam.notam();
            
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> NotamListModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    
    roles[IcaoRole] = "icao";
    roles[IdentRole] = "ident";
    roles[NotamRole] = "notam";
    
    return roles;
}

}} /* namespace Vatsinator::Misc */
