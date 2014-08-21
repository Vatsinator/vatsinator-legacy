/*
    airporttablemodel.cpp
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

#include "db/airportdatabase.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/client/controller.h"

#include "airporttablemodel.h"

AirportTableModel::AirportTableModel(QObject* _parent) :
    QAbstractTableModel(_parent) {}

void
AirportTableModel::addAirport(const Airport* _ap) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __airports.push_back(_ap);
  endInsertRows();
}

int
AirportTableModel::rowCount(const QModelIndex&) const {
  return __airports.size();
}

int
AirportTableModel::columnCount(const QModelIndex&) const {
  return 5;
}

QVariant
AirportTableModel::data(const QModelIndex& _index, int _role) const {
  if (!_index.isValid() || _index.row() >= rowCount() || _index.column() >= columnCount())
    return QVariant();

  switch (_role) {
    case Qt::TextAlignmentRole:

      switch (_index.column()) {
        case Label:
          return QVariant();
        default:
          return Qt::AlignCenter;
      }

    case Qt::ToolTipRole:
      return __arrivalsAndDepartures(_index.row());
      
    case Qt::DisplayRole:
      switch (_index.column()) {
        case Label:
          return __produceLabel(_index.row());
        case Facilities:
          return __produceFacilities(_index.row());
        case Inbounds:
          return QString::number(__airports[_index.row()]->countInbounds());
        case Outbounds:
          return QString::number(__airports[_index.row()]->countOutbounds());
        default:
          return QVariant();
      }

    default:
      return QVariant();
  }
}

QVariant
AirportTableModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
  if (_section >= columnCount() || _orientation == Qt::Vertical || _role != Qt::DisplayRole)
    return QVariant();

  switch (_section) {
    case Label:
      return tr("Airport");
    case Facilities:
      return tr("ATC");
    case Inbounds:
      return tr("Inbound");
    case Outbounds:
      return tr("Outbound");
    default:
      return "";
  }
}

void
AirportTableModel::clear() {
  beginResetModel();
  __airports.clear();
  endResetModel();
}

inline QString
AirportTableModel::__arrivalsAndDepartures(int _row) const {
  return
    tr("Arrivals: %1").arg(QString::number(__airports[_row]->countArrivals())) %
    "<br>" %
    tr("Departures: %1").arg(QString::number(__airports[_row]->countDepartures()));
}

inline QString
AirportTableModel::__produceLabel(int _row) const {
  return
    static_cast< QString >(" ") %
    QString::fromUtf8(__airports[_row]->data()->icao) %
    static_cast< QString >(" ") %
    QString::fromUtf8(__airports[_row]->data()->city);
}

inline QString
AirportTableModel::__produceFacilities(int _row) const {
  Controller::Facilities facilities = __airports[_row]->facilities();

  if (!facilities)
    return "-";

  QString result;

  if (facilities.testFlag(Controller::App))
    result += "APP ";

  if (facilities.testFlag(Controller::Dep))
    result += "DEP ";

  if (facilities.testFlag(Controller::Twr))
    result += "TWR ";

  if (facilities.testFlag(Controller::Gnd))
    result += "GND ";

  if (facilities.testFlag(Controller::Del))
    result += "DEL ";

  if (facilities.testFlag(Controller::Atis))
    result += "ATIS ";

  return result.simplified();
}
