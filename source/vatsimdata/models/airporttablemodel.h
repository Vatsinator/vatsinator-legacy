/*
    airporttablemodel.h
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


#ifndef AIRPORTTABLEMODEL_H
#define AIRPORTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class Airport;

/*
 * This model holds the vector of active airports.
 */
class AirportTableModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit AirportTableModel(QObject* = 0);

  void addAirport(const Airport*);

  int rowCount(const QModelIndex& = QModelIndex()) const;
  int columnCount(const QModelIndex& = QModelIndex()) const;
  QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
  QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;

  void clear();

  inline const QVector<const Airport*> &
  airports() const { return __airports; }

  enum Column {
    Label   = 0,
    Facilities  = 1,
    Outbounds = 2,
    Inbounds  = 3,
    Button    = 4
  };

private:
  QString __arrivalsAndDepartures(int) const;
  QString __produceLabel(int) const;
  QString __produceFacilities(int) const;

  QVector<const Airport*> __airports;


};

#endif // AIRPORTTABLEMODEL_H
