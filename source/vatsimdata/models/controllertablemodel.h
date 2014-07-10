/*
    controllertablemodel.h
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


#ifndef CONTROLLERTABLEMODEL_H
#define CONTROLLERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "vatsimdata/client/controller.h"

class ControllerTableModel : public QAbstractTableModel {
  Q_OBJECT

signals:
  void sorted();

public:
  enum Column {
    Callsign  = 0,
    Name      = 1,
    Frequency = 2,
    Button    = 3
  };
  
  explicit ControllerTableModel(QObject* = 0);

  void add(const Controller*);
  void remove(const Controller*);
  bool contains(const Controller*) const;
  void clear();
  const Controller* findAtcByCallsign(const QString&) const;

  int rowCount(const QModelIndex& = QModelIndex()) const;
  int columnCount(const QModelIndex& = QModelIndex()) const;
  QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
  QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
  void sort(int, Qt::SortOrder = Qt::AscendingOrder);

  inline const QList<const Controller*>& staff() const { return __staff; }
  
private slots:
  void __autoRemove(QObject*);

private:
  QList<const Controller*> __staff;

};

#endif // CONTROLLERTABLEMODEL_H
