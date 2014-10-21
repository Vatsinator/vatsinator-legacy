/*
    airporttablemodel.cpp
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

#include <QtCore>

#include "db/airportdatabase.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/controller.h"

#include "airporttablemodel.h"

AirportTableModel::AirportTableModel(QObject* parent) :
    QAbstractTableModel(parent) {}

void
AirportTableModel::addAirport(const Airport* airport) {
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __airports.push_back(airport);
  endInsertRows();
}

void
AirportTableModel::clear() {
  beginResetModel();
  __airports.clear();
  endResetModel();
}

int
AirportTableModel::rowCount(const QModelIndex& index) const {
  Q_UNUSED(index);
  return __airports.size();
}

int
AirportTableModel::columnCount(const QModelIndex& index) const {
  Q_UNUSED(index);
  return 5;
}

QVariant
AirportTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount())
    return QVariant();

  switch (role) {
    case Qt::TextAlignmentRole:

      switch (index.column()) {
        case Label:
          return QVariant();
        default:
          return Qt::AlignCenter;
      }

    case Qt::ToolTipRole:
      return __arrivalsAndDepartures(index.row());
      
    case Qt::DisplayRole:
      switch (index.column()) {
        case Label:
          return __produceLabel(index.row());
        case Facilities:
          return __produceFacilities(index.row());
        case Inbounds:
          return QString::number(__airports[index.row()]->countInbounds());
        case Outbounds:
          return QString::number(__airports[index.row()]->countOutbounds());
        default:
          return QVariant();
      }

    default:
      return QVariant();
  }
}

QVariant
AirportTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (section >= columnCount() || orientation == Qt::Vertical || role != Qt::DisplayRole)
    return QVariant();

  switch (section) {
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

QString
AirportTableModel::__arrivalsAndDepartures(int row) const {
  return
    tr("Arrivals: %1").arg(QString::number(__airports[row]->countArrivals())) %
    "<br>" %
    tr("Departures: %1").arg(QString::number(__airports[row]->countDepartures()));
}

QString
AirportTableModel::__produceLabel(int row) const {
  return
    static_cast< QString >(" ") %
    QString::fromUtf8(__airports[row]->data()->icao) %
    static_cast< QString >(" ") %
    QString::fromUtf8(__airports[row]->data()->city);
}

QString
AirportTableModel::__produceFacilities(int row) const {
  Controller::Facilities facilities = __airports[row]->facilities();

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
