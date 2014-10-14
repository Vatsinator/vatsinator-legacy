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

#include <algorithm>
#include <QtGui>

#include "db/airportdatabase.h"
#include "vatsimdata/airport.h"

#include "flighttablemodel.h"

FlightTableModel::FlightTableModel(QObject* parent) :
    QAbstractTableModel(parent) {}

void
FlightTableModel::add(const Pilot* pilot) {
  Q_ASSERT(pilot);
  if (!__flights.contains(pilot)) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    __flights << pilot;
    endInsertRows();
    
    connect(pilot, &Pilot::invalid, this, &FlightTableModel::__autoRemove);
  }
}

void
FlightTableModel::remove(const Pilot* pilot) {
  Q_ASSERT(pilot);
  
  for (int i = 0; i < __flights.size(); ++i) {
    if (__flights[i] == pilot) {
      beginRemoveRows(QModelIndex(), i, i);
      __flights.removeAt(i);
      endRemoveRows();
      
      return;
    }
  }
  
  Q_UNREACHABLE();
}

bool
FlightTableModel::contains(const Pilot* pilot) {
  return __flights.contains(pilot);
}

void
FlightTableModel::clear() {
  beginResetModel();
  __flights.clear();
  endResetModel();
}

const Pilot*
FlightTableModel::findFlightByCallsign(const QString& callsign) const {
  for (const Pilot* p: __flights)
    if (p->callsign() == callsign)
      return p;

  return nullptr;
}

int
FlightTableModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
  return __flights.size();
}

int
FlightTableModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
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
FlightTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount())
    return QVariant();

  switch (role) {
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
    case Qt::ToolTipRole:
      return __flights[index.row()]->realName();
    case Qt::ForegroundRole:
      if (__flights[index.row()]->isPrefiledOnly())
        return QBrush(QColor(Qt::gray));
      else
        return QVariant();

    case Qt::DisplayRole:
      const Airport* ap;

      switch (index.column()) {
        case Callsign:
          return __flights[index.row()]->callsign();
        case Name:
          return __flights[index.row()]->realName();
        case From:
          ap = __flights[index.row()]->origin();
          if (ap)
            return QString(
                     QString::fromUtf8(ap->data()->icao) %
                     " " %
                     QString::fromUtf8(ap->data()->city)
                   );
          else
            return __flights[index.row()]->route().origin;

        case To:
          ap = __flights[index.row()]->destination();
          if (ap)
            return QString(
                     QString::fromUtf8(ap->data()->icao) %
                     " " %
                     QString::fromUtf8(ap->data()->city)
                   );
          else
            return __flights[index.row()]->route().destination;

        case Aircraft:
          return __flights[index.row()]->aircraft();
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
FlightTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (section >= columnCount() || orientation == Qt::Vertical || role != Qt::DisplayRole)
    return QVariant();

  switch (section) {
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
FlightTableModel::sort(int column, Qt::SortOrder order) {
  beginResetModel();

  switch (column) {
    case Callsign:
      if (order == Qt::AscendingOrder)
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->callsign() < b->callsign();
        });
      else
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->callsign() > b->callsign();
        });
      break;
      
    case Name:
      if (order == Qt::AscendingOrder)
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->realName() < b->realName();
        });
      else
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->realName() > b->realName();
        });
      break;
      
    case From:
      if (order == Qt::AscendingOrder)
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->route().origin < b->route().origin;
        });
      else
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
        return a->route().origin > b->route().origin;
        });
      break;
      
    case To:
      if (order == Qt::AscendingOrder)
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->route().destination < b->route().destination;
        });
      else
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->route().destination > b->route().destination;
        });
      break;
      
    case Aircraft:
      if (order == Qt::AscendingOrder)
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->aircraft() < b->aircraft();
        });
      else
        std::sort(__flights.begin(), __flights.end(), [] (const Pilot* a, const Pilot* b) {
          return a->aircraft() > b->aircraft();
        });
      break;
  }

  endResetModel();
  
  emit sorted();
}

void
FlightTableModel::__autoRemove() {
  const Pilot* pilot = qobject_cast<const Pilot*>(sender());
  Q_ASSERT(pilot);
  remove(pilot);
}
