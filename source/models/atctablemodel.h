/*
 * atctablemodel.h
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#ifndef ATCTABLEMODEL_H
#define ATCTABLEMODEL_H

#include <QAbstractTableModel>

class Controller;

/**
 * The AtcTableModel class is a model that keeps logged-in ATCs.
 */
class AtcTableModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    /**
     * Column numbers.
     */
    enum Column {
        Callsign  = 0,
        Name      = 1,
        Frequency = 2
    };
    
    /**
     * The default constructor passes \c parent to the QAbstractTableModel.
     */
    explicit AtcTableModel(QObject* parent = nullptr);
    
    /**
     * Adds new Atc to the model.
     * When the client becomes inactive, the pointer is automatically removed.
     * The model does not take ownership over the \c atc.
     *
     * \param atc Controller instance to be added to the model.
     * \sa remove().
     */
    void add(const Controller* atc);
    
    /**
     * Removes the given client from the model.
     * If the model does not contain \c atc, this function throws runtime error.
     *
     * \param atc Controller instance pointer to be removed.
     * \sa add() and contains().
     */
    void remove(const Controller* atc);
    
    /**
     * Checks whether the model contains \c atc or not.
     */
    bool contains(const Controller* atc) const;
    
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
     * Gives direct access to the list of clients.
     */
    inline const QList<const Controller*>& toList() const
    {
        return __staff;
    }
    
private slots:
    void __autoRemove();
    
private:
    QList<const Controller*> __staff;
    
};

#endif // ATCTABLEMODEL_H
