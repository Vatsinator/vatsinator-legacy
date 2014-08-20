/*
    flighttablemodel.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include "db/airportdatabase.h"
#include "vatsimdata/airport.h"

#include "flighttablemodel.h"

FlightTableModel::FlightTableModel(QObject* _parent) :
    QAbstractTableModel(_parent) {}

void
FlightTableModel::add(const Pilot* _p) {
  Q_ASSERT(_p);
  if (!__flights.contains(_p)) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    __flights << _p;
    endInsertRows();
    
    connect(_p,         SIGNAL(destroyed(QObject*)),
            this,       SLOT(__autoRemove(QObject*)), Qt::DirectConnection);
  }
}

void
FlightTableModel::remove(const Pilot* _p) {
  Q_ASSERT(_p);
  
  for (int i = 0; i < __flights.size(); ++i) {
    if (__flights[i] == _p) {
      beginRemoveRows(QModelIndex(), i, i);
      __flights.removeAt(i);
      endRemoveRows();
      
      return;
    }
  }
  
  Q_ASSERT_X(false, "FlightTableModel", "The flight does not exist in the model");
}

bool
FlightTableModel::contains(const Pilot* _p) {
  return __flights.contains(_p);
}

void
FlightTableModel::clear() {
  beginResetModel();
  __flights.clear();
  endResetModel();
}

const Pilot*
FlightTableModel::findFlightByCallsign(const QString& _callsign) const {
  for (const Pilot* p: __flights)
    if (p->callsign() == _callsign)
      return p;

  return nullptr;
}

int
FlightTableModel::rowCount(const QModelIndex&) const {
  return __flights.size();
}

int
FlightTableModel::columnCount(const QModelIndex&) const {
  /* 0 - callsign
   * 1 - name
   * 2 - from
   * 3 - to
   * 4 - aircraft
   * 5 - button
   */
  return 6;
}

QVariant
FlightTableModel::data(const QModelIndex& _index, int _role) const {
  if (!_index.isValid() || _index.row() >= rowCount() || _index.column() >= columnCount())
    return QVariant();

  switch (_role) {
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    case Qt::ToolTipRole:
      return __flights[_index.row()]->realName();
    case Qt::ForegroundRole:
      if (__flights[_index.row()]->isPrefiledOnly())
        return QBrush(QColor(Qt::gray));
      else
        return QVariant();

    case Qt::DisplayRole:
      const Airport* ap;

      switch (_index.column()) {
        case Callsign:
          return __flights[_index.row()]->callsign();
        case Name:
          return __flights[_index.row()]->realName();
        case From:
          ap = __flights[_index.row()]->origin();
          if (ap)
            return QString(
                     QString::fromUtf8(ap->data()->icao) %
                     " " %
                     QString::fromUtf8(ap->data()->city)
                   );
          else
            return __flights[_index.row()]->route().origin;

        case To:
          ap = __flights[_index.row()]->destination();
          if (ap)
            return QString(
                     QString::fromUtf8(ap->data()->icao) %
                     " " %
                     QString::fromUtf8(ap->data()->city)
                   );
          else
            return __flights[_index.row()]->route().destination;

        case Aircraft:
          return __flights[_index.row()]->aircraft();
        case Button: // for non-prefiled-only this will be overriden by QPushButton widget
          return tr("Prefiled");
        default:
          return QVariant();
      }

    default:
      return QVariant();
  }
}

QVariant
FlightTableModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
  if (_section >= columnCount() || _orientation == Qt::Vertical || _role != Qt::DisplayRole)
    return QVariant();

  switch (_section) {
    case Callsign:
      return tr("Callsign");
    case Name:
      return tr("Name");
    case From:
      return tr("From");
    case To:
      return tr("To");
    case Aircraft:
      return tr("Aircraft");
    default:
      return "";
  }
}

void
FlightTableModel::sort(int _column, Qt::SortOrder _order) {
  beginResetModel();

  switch (_column) {
    case Callsign:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->callsign() < _b->callsign();
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->callsign() > _b->callsign();
              }
      );
      break;
      
    case Name:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->realName() < _b->realName();
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->realName() > _b->realName();
              }
      );
      break;
      
    case From:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->route().origin < _b->route().origin;
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->route().origin > _b->route().origin;
              }
      );
      break;
      
    case To:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->route().destination < _b->route().destination;
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->route().destination > _b->route().destination;
              }
      );
      break;
      
    case Aircraft:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->aircraft() < _b->aircraft();
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->aircraft() > _b->aircraft();
              }
      );
      break;
  }

  endResetModel();
  
  emit sorted();
}

void
FlightTableModel::__autoRemove(QObject* _object) {
  Q_ASSERT(_object);
  remove(static_cast<const Pilot*>(_object));
}
