/*
    metarlistmodel.cpp
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "ui/models/roles.h"
#include "vatsimdata/metar.h"

#include "metarlistmodel.h"


MetarListModel::MetarListModel(QObject* parent) :
    QAbstractListModel(parent) {}

MetarListModel::~MetarListModel()
{
    qDeleteAll(__metars);
}

void
MetarListModel::addOrUpdate(const QString& metar)
{
    QString icao = metar.left(4).toUpper();
    
    auto it = std::find_if(__metars.begin(), __metars.end(), [&icao](Metar * m) {
        return m->icao() == icao;
    });
    
    if (it == __metars.end()) {
        beginInsertRows(QModelIndex(), __metars.size(), __metars.size());
        __metars << new Metar(icao, metar);
        endInsertRows();
    } else {
        (*it)->setMetar(metar);
        int n = it - __metars.begin();
        emit dataChanged(createIndex(n, 0), createIndex(n, 0));
    }
}

int
MetarListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return __metars.count();
}

QVariant
MetarListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= __metars.size())
        return QVariant();
        
    switch (role) {
        case Qt::DisplayRole:
            return __metars.at(index.row())->metar();
            
        case Qt::ToolTipRole:
            return __metars.at(index.row())->lastFetchTime();
            
        case MetarRole:
            return QVariant::fromValue(*(__metars.at(index.row())));
            
        default:
            return QVariant();
    }
}

QModelIndexList
MetarListModel::match(const QModelIndex& start, int role, const QVariant& value,
                      int hits, Qt::MatchFlags flags) const
{
    if (role != MetarRole || !start.isValid())
        return QAbstractItemModel::match(start, role, value, hits, flags);
        
    if (value.type() != QVariant::String)
        return QModelIndexList();
        
    QString icao = value.toString().toUpper();
    QModelIndexList matches;
    
    for (int i = start.row(); i < __metars.size() && matches.size() < hits; ++i) {
        if (__metars.at(i)->icao() == icao)
            matches << createIndex(i, 0);
    }
    
    return matches;
}

void
MetarListModel::clear()
{
    beginResetModel();
    qDeleteAll(__metars);
    __metars.clear();
    endResetModel();
}
