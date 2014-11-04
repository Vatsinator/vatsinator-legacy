/*
 * notamlistmodel.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NOTAMLISTMODEL_H
#define NOTAMLISTMODEL_H

#include <QAbstractTableModel>

#include "vatsimdata/notam.h"

/**
 * The NotamListModel class is used to keep NOTAMs.
 * The model keeps NOTAMs only for one ICAO code.
 */
class NotamListModel : public QAbstractTableModel {
  Q_OBJECT
  
  /**
   * This property holds the ICAO code of all the NOTAMs that the model
   * keeps.
   */
  Q_PROPERTY(QString icao READ icao)

public:
  /**
   * Constructs new model for the given _icao_ code and passes _parent_
   * to QAbstractTableModel.
   */
  explicit NotamListModel(QString icao, QObject* parent = nullptr);
  
  /**
   * Adds _notam_ to the model.
   */
  void addNotam(Notam notam);
  
  QVariant data(const QModelIndex& index, int role) const override;
  int rowCount(const QModelIndex& parent) const override;
  int columnCount(const QModelIndex& parent) const override;
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
  
  inline const QString& icao() const { return __icao; }
  
private:
  QString       __icao;
  QList<Notam>  __notams;

};

#endif // NOTAMLISTMODEL_H
