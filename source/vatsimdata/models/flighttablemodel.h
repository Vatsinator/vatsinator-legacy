/*
    flighttablemodel.h
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


#ifndef FLIGHTTABLEMODEL_H
#define FLIGHTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class Pilot;

class FlightTableModel : public QAbstractTableModel {

  /* http://qt-project.org/doc/qt-4.8/qabstracttablemodel.html */
  /*
   * This model holds vector of pilots.
   */

  Q_OBJECT

public:
  explicit FlightTableModel(QObject* = 0);

  void addFlight(const Pilot*);
  void clear();
  const Pilot* findFlightByCallsign(const QString&) const;

  int rowCount(const QModelIndex& = QModelIndex()) const;
  int columnCount(const QModelIndex& = QModelIndex()) const;
  QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
  QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
  void sort(int, Qt::SortOrder = Qt::AscendingOrder);

  inline const QVector< const Pilot* > &
  getFlights() const { return __flights; }

  /* Columns numbers */
  enum Column {
    Callsign  = 0,
    Name    = 1,
    From    = 2,
    To    = 3,
    Aircraft  = 4,
    Button    = 5
  };
  
signals:
  void sorted();

private:
  QVector< const Pilot* > __flights;

};

#endif // FLIGHTTABLEMODEL_H
