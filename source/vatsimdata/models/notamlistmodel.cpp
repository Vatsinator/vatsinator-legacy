/*
 * notamlistmodel.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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
#include "defines.h"

NotamListModel::NotamListModel(QString _icao, QObject* _parent) :
  QAbstractListModel(_parent),
  __icao(std::move(_icao)) {}

void
NotamListModel::addNotam(Notam _notam) {
  __notams << std::move(_notam);
}

QVariant
NotamListModel::data(const QModelIndex& _index, int _role) const {
  switch (_role) {
    case Qt::DisplayRole:
      return __notams.at(_index.row()).notam();
      
    default:
      return QVariant();
  }
}

int
NotamListModel::rowCount(const QModelIndex& _parent) const {
  return __notams.size();
}

