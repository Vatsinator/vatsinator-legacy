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
#include <QList>

#include "vatsimdata/pilot.h"

/**
 * The FlightTableModel class is used to keep Flights (Pilots) in a model.
 */
class FlightTableModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    /**
     * Column numbers.
     */
    enum Column {
        Callsign    = 0,
        Name        = 1,
        From        = 2,
        To          = 3,
        Aircraft    = 4
    };
    
    /**
     * The default constructor passes \c parent to QAbstractTableModel.
     */
    explicit FlightTableModel(QObject* parent = nullptr);
    
    /**
     * Adds new flight to the model.
     * When \c pilot becomes inactive, he is automatically removed from the model.
     * The model does not take ownership over \c pilot.
     *
     * \sa remove().
     */
    void add(const Pilot* pilot);
    
    /**
     * Removes the given \c pilot from the model.
     * The model must contain \c pilot, otherwise an error occurs.
     *
     * \sa add() and remove().
     */
    void remove(const Pilot* pilot);
    
    /**
     * Checks whether the model contains \c pilot or not.
     */
    bool contains(const Pilot* pilot);
    
    /**
     * Looks for the given pilot in the model.
     *
     * \param callsign Callsign of the client to find.
     * \return The Pilot instance or \c nullptr if nothing was found.
     */
    const Pilot* findFlightByCallsign(const QString& callsign) const;
    
    /**
     * \copydoc QAbstractTableModel::rowCount()
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    
    /**
     * \copydoc QAbstractTableModel::columnCount()
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    
    /**
     * \copydoc QAbstractTableModel::data()
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    
    /**
     * \copydoc QAbstractTableModel::headerData()
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    /**
     * \copydoc QAbstractTableModel::sort()
     */
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    
    /**
     * \copydoc QAbstractTableModel::roleNames()
     */
    QHash<int, QByteArray> roleNames() const override;
    
    /**
     * Gives direct access to all flights that the model contains.
     */
    inline const QList<const Pilot*>& toList() const
    {
        return __flights;
    }
    
private slots:
    void __autoRemove();
    
private:
    QList<const Pilot*> __flights;
    
};

Q_DECLARE_METATYPE(FlightTableModel*)

#endif // FLIGHTTABLEMODEL_H
