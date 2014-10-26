/*
    controllertablemodel.h
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


#ifndef CONTROLLERTABLEMODEL_H
#define CONTROLLERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "vatsimdata/controller.h"

/**
 * The ControllerTableModel class is a model that keeps logged-in ATC clients.
 */
class ControllerTableModel : public QAbstractTableModel {
  Q_OBJECT

signals:
  /**
   * This signal is emitted whenever user sorts the model.
   * \todo Remove.
   */
  void sorted();

public:
  enum Column {
    Callsign  = 0,
    Name      = 1,
    Frequency = 2
  };
  
  explicit ControllerTableModel(QObject* parent = nullptr);

  /**
   * Adds new Controller to the model.
   * When the client becomes inactive, the pointer is automatically removed.
   * The model does not take ownership over the _atc_.
   * 
   * \param atc Controller instance to be added to the model.
   * \sa remove().
   */
  void add(const Controller* atc);
  
  /**
   * Removes the given client from the model.
   * If the model does not contain _atc_, this function throws runtime error.
   * 
   * \param atc Controller instance pointer to be removed.
   * \sa add() and contains().
   */
  void remove(const Controller* atc);
  
  /**
   * Checks whether the model contains _atc_ or not.
   */
  bool contains(const Controller* atc) const;
  
  /**
   * Empties the model.
   * \todo Remove.
   */
  void clear();
  
  /**
   * Looks for the given client. The complexity is linear.
   * 
   * \return Found client instance pointer or _nullptr_.
   */
  const Controller* findAtcByCallsign(const QString& callsign) const;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

  /**
   * Gives direct access to the list of clients.
   */
  inline const QList<const Controller*>& staff() const { return __staff; }
  
private slots:
  void __autoRemove();

private:
  QList<const Controller*> __staff;

};

#endif // CONTROLLERTABLEMODEL_H
