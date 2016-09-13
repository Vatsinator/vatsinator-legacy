/*
 * clientlistmodel.h
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#ifndef MISC_CLIENTLISTMODEL_H
#define MISC_CLIENTLISTMODEL_H

#include "miscexport.h"
#include "core/airportobject.h"
#include "core/firobject.h"
#include <QAbstractListModel>

namespace Vatsinator { namespace Core { class Client; }}

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The ClientListModel class stores \c Client instances.
 */
class __VtrMiscApi__ ClientListModel : public QAbstractListModel {
    Q_OBJECT

public:
    /**
     * Creates new, empty model.
     */
    explicit ClientListModel(QObject* parent = nullptr);
    
    /**
     * Destroys the model.
     */
    virtual ~ClientListModel();
    
    /**
     * Defines whether the model should be sorted or not.
     */
    void setSorted(bool sorted);
    
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
     * Creates a new model that tracks all clients of the given \c airport.
     */
    static ClientListModel* clients(const Core::AirportObject* airport,
                                    QObject* parent = nullptr);

    /**
     * Creates a new model that tracks all clients of the given \c fir.
     */
    static ClientListModel* clients(const Core::FirObject* fir,
                                    QObject* parent = nullptr);

private:
    /**
     * Adds a new client.
     */
    void add(const Core::Client* client);
    
private slots:
    /**
     * Removes the client.
     */
    void remove(QObject* obj);
    
private:
    QList<const Core::Client*> m_clients;
    bool m_sorted= false;
    
}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::ClientListModel*)

#endif // MISC_CLIENTLISTMODEL_H
