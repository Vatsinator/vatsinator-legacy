/*
    bookedatctablemodel.h
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


#ifndef BOOKEDATCTABLEMODEL_H
#define BOOKEDATCTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class BookedController;

class BookedAtcTableModel : public QAbstractTableModel {
  
  Q_OBJECT
  
public:
  explicit BookedAtcTableModel (QObject* = 0);
  virtual ~BookedAtcTableModel();
  
  void addStaff(const BookedController*);
  void clear();
  
  int rowCount(const QModelIndex& = QModelIndex()) const;
  int columnCount(const QModelIndex& = QModelIndex()) const;
  QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
  QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
  
  enum Column {
    Callsign  = 0,
    Name      = 1,
    Date      = 2,
    Hours     = 3
  };
  
private:
  QVector< const BookedController* > __staff;
  
};

#endif // BOOKEDATCTABLEMODEL_H
