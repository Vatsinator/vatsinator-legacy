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

BookedAtcTableModel::BookedAtcTableModel(QObject* _parent) :
    QAbstractTableModel(_parent) {}

BookedAtcTableModel::~BookedAtcTableModel() {
  for (const BookedController* bc: __staff)
    delete bc;
}

void
BookedAtcTableModel::add(const BookedController* _bc) {
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
    case Qt::ForegroundRole:
      if (__staff[_index.row()]->isTrainingSession())
        return QBrush(Qt::blue);
      return QVariant();
    case Qt::DisplayRole:
      
      switch (_index.column()) {
        case Callsign:
          return __staff[_index.row()]->callsign();
        case Name:
          return __staff[_index.row()]->realName();
        case Date:
          return __staff[_index.row()]->dateBooked().toString("dd MMM yyyy");
        case Hours:
          return QString(
              __staff[_index.row()]->bookedFrom().toString("hh:mm") %
              static_cast< QString >(" - ") %
              __staff[_index.row()]->bookedTo().toString("hh:mm"));
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
BookedAtcTableModel::sort(int _column, Qt::SortOrder _order) {
  beginResetModel();
  
  switch(_column) {
    case Callsign:
      qSort(__staff.begin(), __staff.end(),
            _order == Qt::AscendingOrder ?
              [](const BookedController* _a, const BookedController* _b) -> bool {
                return _a->callsign() < _b->callsign();
              } :
              [](const BookedController* _a, const BookedController* _b) -> bool {
                return _a->callsign() > _b->callsign();
              }
      );
      
      break;
      
    case Name:
      qSort(__staff.begin(), __staff.end(),
            _order == Qt::AscendingOrder ?
              [](const BookedController* _a, const BookedController* _b) -> bool {
                return _a->realName() < _b->realName();
              } :
              [](const BookedController* _a, const BookedController* _b) -> bool {
                return _a->realName() > _b->realName();
              }
      );
      
      break;
      
    case Date:
      qSort(__staff.begin(), __staff.end(),
            _order == Qt::AscendingOrder ?
              [](const BookedController* _a, const BookedController* _b) -> bool {
                return _a->dateBooked() < _b->dateBooked();
              } :
              [](const BookedController* _a, const BookedController* _b) -> bool {
                return _a->dateBooked() > _b->dateBooked();
              }
      );
      
      break;
      
    case Hours:
      qSort(__staff.begin(), __staff.end(),
            _order == Qt::AscendingOrder ?
            [](const BookedController* _a, const BookedController* _b) -> bool {
              return QDateTime(_a->dateBooked(), _a->bookedFrom()) <
                  QDateTime(_b->dateBooked(), _b->bookedFrom());
            } :
            [](const BookedController* _a, const BookedController* _b) -> bool {
              return QDateTime(_a->dateBooked(), _a->bookedFrom()) >
                  QDateTime(_b->dateBooked(), _b->bookedFrom());
            }
      );
      
      break;
  }
  
  endResetModel();
  
  emit sorted();
}

