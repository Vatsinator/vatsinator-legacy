/*
 * atcbookinglistmodel.h
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

#ifndef MISC_ATCBOOKINGLISTMODEL_H
#define MISC_ATCBOOKINGLISTMODEL_H

#include "misc/atcbooking.h"
#include "misc/vtrmisc_export.h"
#include <QtCore/QAbstractListModel>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 *
 * A model for keeping \ref AtcBooking instances.
 */
class VTRMISC_EXPORT AtcBookingListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    /**
     * Creates an empty model.
     */
    explicit AtcBookingListModel(QObject* parent = nullptr);
    
    /**
     * Creates a model from the given \c AtcBooking list.
     */
    explicit AtcBookingListModel(const QList<AtcBooking>& bookings,
                                 QObject* parent = nullptr);
    
    /**
     * Destroys the model.
     */
    virtual ~AtcBookingListModel();
    
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
    
private:
    QList<AtcBooking> m_bookings;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::AtcBookingListModel*)

#endif // MISC_ATCBOOKINGLISTMODEL_H
