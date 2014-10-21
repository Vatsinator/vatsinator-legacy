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

#include <algorithm>
#include <QtGui>

#include "modules/vatbook/bookedcontroller.h"

#include "bookedatctablemodel.h"

BookedAtcTableModel::BookedAtcTableModel(QObject* parent) :
    QAbstractTableModel(parent) {}

BookedAtcTableModel::~BookedAtcTableModel() {
  qDeleteAll(__staff);
}

void
BookedAtcTableModel::add(const BookedController* bc) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __staff << bc;
  endInsertRows();
}

void
BookedAtcTableModel::clear() {
  beginResetModel();
  qDeleteAll(__staff);
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
BookedAtcTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount())
    return QVariant();
  
  switch (role) {
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    case Qt::ForegroundRole:
      if (__staff[index.row()]->isTrainingSession())
        return QBrush(Qt::blue);
      return QVariant();
    case Qt::DisplayRole:
      
      switch (index.column()) {
        case Callsign:
          return __staff[index.row()]->callsign();
        case Name:
          return __staff[index.row()]->realName();
        case Date:
          return __staff[index.row()]->dateBooked().toString("dd MMM yyyy");
        case Hours:
          return QString(
              __staff[index.row()]->bookedFrom().toString("hh:mm") %
              static_cast< QString >(" - ") %
              __staff[index.row()]->bookedTo().toString("hh:mm"));
        default:
          return QVariant();
      }
      
    default:
      return QVariant();
  }
}

QVariant
BookedAtcTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
BookedAtcTableModel::sort(int column, Qt::SortOrder order) {
  beginResetModel();
  
  switch(column) {
    case Callsign:
      if (order == Qt::AscendingOrder)
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return a->callsign() < b->callsign();
        });
      else
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return a->callsign() > b->callsign();
        });
      break;
      
    case Name:
      if (order == Qt::AscendingOrder)
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return a->realName() < b->realName();
        });
      else
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return a->realName() > b->realName();
        });
      break;
      
    case Date:
      if (order == Qt::AscendingOrder)
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return a->dateBooked() < b->dateBooked();
        });
      else
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return a->dateBooked() > b->dateBooked();
        });
      break;
      
    case Hours:
      if (order == Qt::AscendingOrder)
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return QDateTime(a->dateBooked(), a->bookedFrom()) <
            QDateTime(b->dateBooked(), b->bookedFrom());
        });
      else
        std::sort(__staff.begin(), __staff.end(), [] (const BookedController* a, const BookedController* b) {
          return QDateTime(a->dateBooked(), a->bookedFrom()) <
            QDateTime(b->dateBooked(), b->bookedFrom());
        });
      break;
  }
  
  endResetModel();
  
  emit sorted();
}
