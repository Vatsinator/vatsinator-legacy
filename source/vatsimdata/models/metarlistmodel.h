/*
    metarlistmodel.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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
#include "singleton.h"

#include "vatsimdata/metar.h"

class HttpHandler;

class MetarListModel : public QAbstractListModel, public Singleton< MetarListModel > {

  Q_OBJECT

public:
  MetarListModel(HttpHandler*, QObject* = 0);

  /**
   * Starts fetching the METAR.
   * After done, emits newMetarsAvailable().
   * @param icao Airport ICAO code.
   */
  void fetchMetar(const QString&);

  /**
   * Looks for the METAR.
   * @param icao Airport ICAO code.
   * @return Const ptr to found METAR or NULL.
   */
  const Metar* find(const QString&) const;

  /* Two QAbstractListModel-reimplemented functions */
  int rowCount(const QModelIndex& = QModelIndex()) const;
  QVariant data(const QModelIndex&, int) const;

public slots:
  void updateAllMetars();
  void clear();

signals:
  void newMetarsAvailable();
  void noMetar();

private:
  void __addMetar(const QString&);

  bool __matches(const QString&);

  QQueue< QString > __requests;

  QList< Metar > __metarList;

  HttpHandler* __myHttpHandler;

private slots:
  void __gotMetar(const QString&);

};

#endif // METARLISTMODEL_H
