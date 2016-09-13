/*
 * flightlistmodel.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#ifndef MISC_FLIGHTLISTMODEL_H
#define MISC_FLIGHTLISTMODEL_H

#include "miscexport.h"
#include "core/airportobject.h"
#include "core/firobject.h"
#include <QAbstractListModel>

namespace Vatsinator { namespace Core { class Pilot; }}

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The FlightListModel class wraps a list of flights in the Qt's item model.
 * When created using either \ref inbound() or \ref outbound methods, it
 * updates automatically.
 */
class __VtrMiscApi__ FlightListModel : public QAbstractListModel {
    Q_OBJECT
    template<typename Pred> friend FlightListModel* fromClientList(const Core::ClientList*, QObject*, Pred);
    
public:    
    /**
     * Creates an empty model.
     */
    explicit FlightListModel(QObject* parent = nullptr);
    
    /**
     * Destroys the model.
     */
    virtual ~FlightListModel();
    
    /**
     * \copydoc QAbstractListModel::rowCount
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    
    /**
     * \copydoc QAbstractListModel::columnCount
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    
    /**
     * \copydoc QAbstractListModel::data
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    
    /**
     * \copydoc QAbstractListModel::headerData
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    
    /**
     * \copydoc QAbstractListModel::roleNames
     */
    QHash<int, QByteArray> roleNames() const override;
    
    /**
     * Creates a new model that tracks inbound flights of the given \c airport.
     */
    static FlightListModel* inbound(const Core::AirportObject* airport,
                                    QObject* parent = nullptr);
    
    /**
     * Creates a new model that tracks outbound flights of the given \c airport.
     */
    static FlightListModel* outbound(const Core::AirportObject* airport,
                                     QObject* parent = nullptr);
    
    /**
     * Creates a new model that tracks inbount and outbound flights of the 
     * given \c fir.
     */
    static FlightListModel* around(const Core::FirObject* fir,
                                   QObject* parent = nullptr);

private:
    /**
     * Handles new client.
     */
    void add(const Core::Client* client);
    
private slots:
    /**
     * Removes the client.
     */
    void remove(QObject* obj);
    
private:
    QList<const Core::Pilot*> m_flights;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::FlightListModel*)

#endif // MISC_FLIGHTLISTMODEL_H
