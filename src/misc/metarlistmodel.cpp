/*
 * metarlistmodel.h
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

#include "metarlistmodel.h"

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Misc {

MetarListModel::MetarListModel(const QList<Core::Metar>& metars, QObject* parent) :
    QAbstractListModel(parent),
    m_metars(metars) {}

MetarListModel::MetarListModel(QObject* parent) :
    QAbstractListModel(parent) {}

MetarListModel::~MetarListModel() {}

int MetarListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_metars.count();
}

QVariant MetarListModel::data(const QModelIndex& index, int role) const
{
    Metar metar = m_metars.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return metar.metar();

        default:
            return QVariant();
    }
}

}} /* namespace Vatsinator::Misc */
