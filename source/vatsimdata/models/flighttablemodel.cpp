/*
    flighttablemodel.cpp
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

#include "db/airportdatabase.h"

#include "flighttablemodel.h"
#include "defines.h"

FlightTableModel::FlightTableModel(QObject* _parent) :
    QAbstractTableModel(_parent) {}

void
FlightTableModel::addFlight(const Pilot* _p) {
  Q_ASSERT(_p);
  if (!__flights.contains(_p)) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    __flights.push_back(_p);
    endInsertRows();
  }
}

void
FlightTableModel::clear() {
  beginResetModel();
  __flights.clear();
  endResetModel();
}

const Pilot*
FlightTableModel::findFlightByCallsign(const QString& _callsign) const {
for (const Pilot * p: __flights)
    if (p->getCallsign() == _callsign)
      return p;

  return NULL;
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
      return __flights[_index.row()]->getRealName();
    case Qt::ForegroundRole:

      if (__flights[_index.row()]->isPrefiledOnly())
        return QBrush(QColor(Qt::gray));
      else
        return QVariant();

    case Qt::DisplayRole:
      const AirportRecord* ap;

      switch (_index.column()) {
        case Callsign:
          return __flights[_index.row()]->getCallsign();
        case Name:
          return __flights[_index.row()]->getRealName();
        case From:
          ap = AirportDatabase::getSingleton().find(__flights[_index.row()]->getRoute().origin);

          if (ap)
            return QString(
                     QString::fromUtf8(ap->icao) %
                     " " %
                     QString::fromUtf8(ap->city)
                   );
          else
            return __flights[_index.row()]->getRoute().origin;

        case To:
          ap = AirportDatabase::getSingleton().find(__flights[_index.row()]->getRoute().destination);

          if (ap)
            return QString(
                     QString::fromUtf8(ap->icao) %
                     " " %
                     QString::fromUtf8(ap->city)
                   );
          else
            return __flights[_index.row()]->getRoute().destination;

        case Aircraft:
          return __flights[_index.row()]->getAircraft();
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
                return _a->getCallsign() < _b->getCallsign();
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getCallsign() > _b->getCallsign();
              }
      );
      
      break;
      
    case Name:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getRealName() < _b->getRealName();
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getRealName() > _b->getRealName();
              }
      );
      
      break;
      
    case From:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getRoute().origin < _b->getRoute().origin;
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getRoute().origin > _b->getRoute().origin;
              }
      );
      
      break;
      
    case To:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getRoute().destination < _b->getRoute().destination;
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getRoute().destination > _b->getRoute().destination;
              }
      );
      
      break;
      
    case Aircraft:
      qSort(__flights.begin(), __flights.end(),
            _order == Qt::AscendingOrder ?
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getAircraft() < _b->getAircraft();
              } :
              [](const Pilot* _a, const Pilot* _b) -> bool {
                return _a->getAircraft() > _b->getAircraft();
              }
      );
      
      break;
  }
      

  endResetModel();
  
  emit sorted();
}

