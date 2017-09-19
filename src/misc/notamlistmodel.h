/*
 * notamlistmodel.h
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

#ifndef MISC_NOTAMLISTMODEL_H
#define MISC_NOTAMLISTMODEL_H

#include "misc/notam.h"
#include "misc/v8rmisc_export.h"
#include <QtCore/QAbstractListModel>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The NotamListModel represents a list of NOTAMs in a model.
 */
class V8RMISC_EXPORT NotamListModel : public QAbstractListModel {
    Q_OBJECT

public:
    /**
     * Creates an empty model.
     */
    explicit NotamListModel(QObject* parent = nullptr);
    
    /**
     * Creates a model from the given NOTAM list.
     */
    explicit NotamListModel(const QList<Notam>& notams,
                            QObject* parent = nullptr);
    
    /**
     * Destroys the model.
     */
    virtual ~NotamListModel();
    
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
     * \copydoc QAbstractListModel::roleNames
     */
    QHash<int, QByteArray> roleNames() const override;
    
private:
    QList<Notam> m_notams;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::NotamListModel*)

#endif // MISC_NOTAMLISTMODEL_H
