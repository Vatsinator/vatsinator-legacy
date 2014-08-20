/*
    controllertablemodel.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include "controllertablemodel.h"

ControllerTableModel::ControllerTableModel(QObject* _parent) :
    QAbstractTableModel(_parent) {}

void
ControllerTableModel::add(const Controller* _c) {
  Q_ASSERT(_c);
  
  if (!__staff.contains(_c)) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    __staff << _c;
    endInsertRows();
    
    connect(_c,         SIGNAL(destroyed(QObject*)),
            this,       SLOT(__autoRemove(QObject*)), Qt::DirectConnection);
  }
}

void
ControllerTableModel::remove(const Controller* _c) {
  Q_ASSERT(_c);
  
  for (int i = 0; i < __staff.size(); ++i) {
    if (__staff[i] == _c) {
      beginRemoveRows(QModelIndex(), i, i);
      __staff.removeAt(i);
      endRemoveRows();
      
      return;
    }
  }
  
  Q_ASSERT_X(false, "ControllerTableModel", "The ATC does not exist in the model");
}

bool
ControllerTableModel::contains(const Controller* _c) const {
  return __staff.contains(_c);
}

void
ControllerTableModel::clear() {
  beginResetModel();
  __staff.clear();
  endResetModel();
}

const Controller *
ControllerTableModel::findAtcByCallsign(const QString& _callsign) const {
  for (const Controller* c: __staff)
    if (c->callsign() == _callsign)
      return c;
  
  return nullptr;
}

int
ControllerTableModel::rowCount(const QModelIndex& ) const {
  return __staff.size();
}

int
ControllerTableModel::columnCount(const QModelIndex& ) const {
  /* 0 - callsign
   * 1 - name
   * 2 - freq
   * 3 - button
   */
  return 4;
}

QVariant
ControllerTableModel::data(const QModelIndex& _index, int _role) const {
  if (!_index.isValid() || _index.row() >= rowCount() || _index.column() >= columnCount())
    return QVariant();

  switch (_role) {
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    case Qt::ToolTipRole:
      return Controller::ratings[__staff[_index.row()]->rating()];
    case Qt::DisplayRole:

      switch (_index.column()) {
        case Callsign:
          return __staff[_index.row()]->callsign();
        case Name:
          return __staff[_index.row()]->realName();
        case Frequency:
          return __staff[_index.row()]->frequency();
        default:
          return QVariant();
      }

    default:
      return QVariant();
  }
}

QVariant
ControllerTableModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
  if (_section >= columnCount() || _orientation == Qt::Vertical || _role != Qt::DisplayRole)
    return QVariant();

  switch (_section) {
    case 0:
      return tr("Callsign");
    case 1:
      return tr("Name");
    case 2:
      return tr("Frequency");
    default:
      return "";
  }
}

void
ControllerTableModel::sort(int _column, Qt::SortOrder _order) {
  beginResetModel();

  switch (_column) {
    case Callsign:
      qSort(__staff.begin(), __staff.end(),
            _order == Qt::AscendingOrder ?
              [](const Controller* _a, const Controller* _b) -> bool {
                return _a->callsign() < _b->callsign();
              } :
              [](const Controller* _a, const Controller* _b) -> bool {
                return _a->callsign() > _b->callsign();
              }
      );
      
      break;
    case Name:
      qSort(__staff.begin(), __staff.end(),
            _order == Qt::AscendingOrder ?
              [](const Controller* _a, const Controller* _b) -> bool {
                return _a->realName() < _b->realName();
              } :
              [](const Controller* _a, const Controller* _b) -> bool {
                return _a->realName() > _b->realName();
              }
      );
      
      break;
  }

  endResetModel();
  
  emit sorted();
}

void
ControllerTableModel::__autoRemove(QObject* _object) {
  Q_ASSERT(_object);
  remove(static_cast<const Controller*>(_object));
}
