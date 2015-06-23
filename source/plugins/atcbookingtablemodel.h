/*
 * atcbookingtablemodel.h
 * Copyright (C) 2014  Michal Garapich <michal@garapich.pl>
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

#ifndef ATCBOOKINGTABLEMODEL_H
#define ATCBOOKINGTABLEMODEL_H

#include <QAbstractTableModel>

class AtcBooking;

/**
 * The AtcBookingTableModel is a model that holds bookings.
 */
class AtcBookingTableModel : public QAbstractTableModel {
    Q_OBJECT
    
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
     * The default constructor passes _parent_ to QAbstractTableModel.
     */
    explicit AtcBookingTableModel(QObject* parent = nullptr);
    
    /**
     * Adds a new booking to the model.
     * The model takes ownership over the added item.
     */
    void add(AtcBooking* booking);
    
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    
private:
    QList<AtcBooking*> __bookings;
};

#endif // ATCBOOKINGTABLEMODEL_H
