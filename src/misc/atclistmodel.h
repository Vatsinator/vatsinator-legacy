/*
 * atclistmodel.h
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

#ifndef MISC_ATCLISTMODEL_H
#define MISC_ATCLISTMODEL_H

#include "core/airport.h"
#include "core/fir.h"
#include "misc/v8rmisc_export.h"
#include <QtCore/QAbstractListModel>

namespace Vatsinator { namespace Core { class Atc; }}

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The AtcListModel class stores list of ATCs in a model.
 */
class V8RMISC_EXPORT AtcListModel : public QAbstractListModel {
    Q_OBJECT

public:
    /**
     * Creates an empty model.
     */
    explicit AtcListModel(QObject* parent = nullptr);
    
    /**
     * Destroys the model.
     */
    virtual ~AtcListModel();
    
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
     * Creates a new model that tracks all ATCs of the given \c airport.
     */
    static AtcListModel* staff(const Core::Airport* airport,
                               QObject* parent = nullptr);
    
    /**
     * Creates a new model that tracks all ATCs of the given \c fir.
     */
    static AtcListModel* staff(const Core::Fir* fir,
                               QObject* parent = nullptr);

    /**
     * Creates a new model that tracks all ATCs connected to the \c server.
     */
    static AtcListModel* all(const Core::ServerTracker* server,
                             QObject* parent = nullptr);
    
private:
    /**
     * Handles new client.
     */
    void add(const Core::Atc* atc);
    
    static AtcListModel* staffImpl(const Core::ClientList* list, QObject* parent = nullptr);

private slots:
    /**
     * Removes the client.
     */
    void remove(QObject* obj);
    
private:
    QList<const Core::Atc*> m_atcs;
    
}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::AtcListModel*)

#endif // MISC_ATCLISTMODEL_H
