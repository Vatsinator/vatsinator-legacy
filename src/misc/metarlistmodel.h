/*
 * metarlistmodel.h
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

#ifndef MISC_METARLISTMODEL_H
#define MISC_METARLISTMODEL_H

#include "core/metar.h"
#include "misc/vtrmisc_export.h"
#include <QtCore/QAbstractListModel>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 *
 * The MetarListModel represents a list of METARs in a model.
 */
class VTRMISC_EXPORT MetarListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit MetarListModel(const QList<Core::Metar>& metars, QObject* parent = nullptr);
    explicit MetarListModel(QObject* parent = nullptr);
    virtual ~MetarListModel();

    /**
     * \copydoc QAbstractListModel::rowCount
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * \copydoc QAbstractListModel::data
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QList<Core::Metar> m_metars;

}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_METARLISTMODEL_H
