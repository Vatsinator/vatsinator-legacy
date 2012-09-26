/*
    bookedatctablemodel.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include <QtGui>

#include "modules/vatbook/bookedcontroller.h"

#include "bookedatctablemodel.h"
#include "defines.h"

BookedAtcTableModel::BookedAtcTableModel(QObject* _parent) :
    QAbstractTableModel(_parent) {}

void
BookedAtcTableModel::addStaff(const BookedController* _bc) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __staff.push_back(_bc);
  endInsertRows();
}

void
BookedAtcTableModel::clear() {
  beginResetModel();
  for (const BookedController* bc: __staff)
    delete bc;
  __staff.clear();
  endResetModel();
}

int
BookedAtcTableModel::rowCount(const QModelIndex&) const {
  return __staff.size();
}

int
BookedAtcTableModel::columnCount(const QModelIndex&) const {
  /*
   * 0 - callsign
   * 1 - realName
   * 2 - date booked
   * 3 - hours booked
   */
  return 4;
}

QVariant
BookedAtcTableModel::data(const QModelIndex& _index, int _role) const {
  if (!_index.isValid() || _index.row() >= rowCount() || _index.column() >= columnCount())
    return QVariant();
  
  switch (_role) {
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    case Qt::DisplayRole:
      
      switch (_index.column()) {
        case Callsign:
          return __staff[_index.row()]->callsign;
        case Name:
          return __staff[_index.row()]->realName;
        case Date:
          return __staff[_index.row()]->dateBooked.toString("dd MMM yyyy");
        case Hours:
          return QString(
              __staff[_index.row()]->bookedFrom.toString("hh:mm") %
              static_cast< QString >(" - ") %
              __staff[_index.row()]->bookedTo.toString("hh:mm"));
        default:
          return QVariant();
      }
      
    default:
      return QVariant();
  }
}

QVariant
BookedAtcTableModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
  if (_section >= columnCount() || _orientation == Qt::Vertical || _role != Qt::DisplayRole)
    return QVariant();
  
  switch (_section) {
    case 0:
      return tr("Callsign");
    case 1:
      return tr("Name");
    case 2:
      return tr("Date");
    case 3:
      return tr("Hours");
    default:
      return QVariant();
  }
}

