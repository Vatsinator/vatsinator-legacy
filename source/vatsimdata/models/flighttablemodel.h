/*
    flighttablemodel.h
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


#ifndef FLIGHTTABLEMODEL_H
#define FLIGHTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "vatsimdata/pilot.h"

/**
 * The FlightTableModel class is used to keep Flights (Pilots) in a model.
 */
class FlightTableModel : public QAbstractTableModel {
  Q_OBJECT
  
signals:
  /**
   * This signal is emitted whenever user sorts the model.
   * \todo Remove.
   */
  void sorted();

public:
  /* Column numbers */
  enum Column {
    Callsign    = 0,
    Name        = 1,
    From        = 2,
    To          = 3,
    Aircraft    = 4,
    Button      = 5
  };
  
  /**
   * The default constructor passes _parent_ to QAbstractTableModel.
   */
  explicit FlightTableModel(QObject* parent = nullptr);

  /**
   * Adds new flight to the model.
   * When _pilot_ becomes inactive, he is automatically removed from the model.
   * The model does not take ownership over _pilot_.
   * 
   * \sa remove().
   */
  void add(const Pilot* pilot);
  
  /**
   * Removes the given _pilot_ from the model.
   * The model must contain _pilot_, otherwise an error occurs.
   * 
   * \sa add() and remove().
   */
  void remove(const Pilot* pilot);
  
  /**
   * Checks whether the model contains _pilot_ or not.
   */
  bool contains(const Pilot* pilot);
  
  /**
   * Removes all pointers.
   * 
   * \todo Remove.
   */
  void clear();
  
  /**
   * Looks for the given pilot in the model.
   * 
   * \param callsign Callsign of the client to find.
   * \return The Pilot instance or _nullptr_ if nothing was found.
   */
  const Pilot* findFlightByCallsign(const QString& callsign) const;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
  
  /**
   * Gives direct access to all flights that the model contains.
   */
  inline const QList<const Pilot*>& flights() const { return __flights; }
  
private slots:
  void __autoRemove();
  
private:
  QList<const Pilot*> __flights;

};

#endif // FLIGHTTABLEMODEL_H
