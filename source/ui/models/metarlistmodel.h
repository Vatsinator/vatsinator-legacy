/*
    metarlistmodel.h
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

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


#ifndef METARLISTMODEL_H
#define METARLISTMODEL_H

#include <QAbstractListModel>
#include <QQueue>

class Metar;

/**
 * The MetarListModel class is a model that keeps METARs.
 */
class MetarListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    /**
     * The constructor passes \c parent to the \c QAbstractListModel.
     */
    MetarListModel(QObject* parent = nullptr);
    
    virtual ~MetarListModel();
    
    /**
     * Adds the given METAR object to the model.
     */
    void addMetar(Metar* metar);
    
    /**
     * \copydoc QAbstractListModel::rowCount()
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    
    /**
     * \copydoc QAbstractListModel::data()
     */
    QVariant data(const QModelIndex& index, int role) const override;
    
    /**
     * \copydoc QAbstractListModel::match()
     */
    QModelIndexList match(const QModelIndex& start, int role,
                          const QVariant& value, int hits = 1,
                          Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)) const override;
                          
public slots:
    void clear();
    
private slots:
    void __metarUpdated(const QString &metar);
    
private:
    QList<Metar*> __metars;
    
};

#endif // METARLISTMODEL_H
