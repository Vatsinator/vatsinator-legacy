/*
    airporttablemodel.h
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


#ifndef AIRPORTTABLEMODEL_H
#define AIRPORTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

class Airport;

/**
 * The AirportTableModel holds airports.
 */
class AirportTableModel : public QAbstractTableModel {
  Q_OBJECT

public:
  /**
   * The default constructor passes _parent_ to QAbstractTableModel.
   */
  explicit AirportTableModel(QObject* parent = nullptr);

  /**
   * Adds new airport to the model.
   */
  void addAirport(const Airport* airport);
  
  /**
   * Removes all pointers.
   * \todo Remove.
   */
  void clear();

  int rowCount(const QModelIndex& index = QModelIndex()) const override;
  int columnCount(const QModelIndex& index = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  /**
   * Gives direct access to all airports.
   */
  inline const QList<const Airport*>& airports() const { return __airports; }

  enum Column {
    Label       = 0,
    Facilities  = 1,
    Outbounds   = 2,
    Inbounds    = 3,
    Button      = 4
  };

private:
  QString __arrivalsAndDepartures(int row) const;
  QString __produceLabel(int row) const;
  QString __produceFacilities(int row) const;

  QList<const Airport*> __airports;


};

#endif // AIRPORTTABLEMODEL_H
