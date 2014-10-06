/*
    bookedatctablemodel.h
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


#ifndef BOOKEDATCTABLEMODEL_H
#define BOOKEDATCTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

class BookedController;

/**
 * The BookedAtcTableModel is used to hold bookings.
 * These bookings are displayed in AirportDetailsWindow and
 * FirDetailsWindow.
 */
class BookedAtcTableModel : public QAbstractTableModel {
  Q_OBJECT
  
signals:
  /**
   * Emited just after the model is sorted.
   */
  void sorted();
  
public:
  /**
   * The Column enum describes various columns in which the model data
   * is displayed.
   */
  enum Column {
    Callsign  = 0, /**< Client's callsign */
    Name      = 1, /**< Real name */
    Date      = 2, /**< Date of booking */
    Hours     = 3  /**< Hours of booking */
  };
  
  /**
   * Defaults constructor, passes _parent_ to QAbstractTableModel.
   */
  explicit BookedAtcTableModel(QObject* parent = nullptr);
  
  /**
   * The destructor.
   */
  virtual ~BookedAtcTableModel();
  
  /**
   * Adds the given booking to the model.
   * 
   * \param bc Item to be added; the model takes the ownership.
   */
  void add(const BookedController* bc);
  
  /**
   * Clears the model.
   */
  void clear();
  
  /**
   * \overload
   */
  int rowCount(const QModelIndex& = QModelIndex()) const override;
  
  /**
   * \overload
   */
  int columnCount(const QModelIndex& = QModelIndex()) const override;
  
  /**
   * \overload
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  
  /**
   * \overload
   */
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  
  /**
   * \overload
   */
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
  
private:
  QList<const BookedController*> __staff;
  
};

#endif // BOOKEDATCTABLEMODEL_H
