/*
    metarlistmodel.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

class PlainTextDownloader;

/**
 * The MetarListModel class is a model that keeps METAR reports.
 * 
 * \todo Split to MetarListModel and MetarUpdater.
 */
class MetarListModel : public QAbstractListModel, public Singleton<MetarListModel> {
  Q_OBJECT

signals:
  /**
   * This signal is emitted whenever a new METAR report is available.
   */
  void newMetarsAvailable();
  
  /**
   * This signal is emitted in order to idicate that the requested METAR
   * is unavailable.
   */
  void noMetar(QString icao);

public:
  /**
   * Constructs new MetarListModel instance that will use the given _downloader_.
   * Passes _parent_ to QAbstractListModel.
   */
  MetarListModel(PlainTextDownloader* downloader, QObject* parent = 0);

  /**
   * Starts fetching the METAR.
   * After done, emits _newMetarsAvailable()_ signal.
   * \param icao Airport ICAO code.
   */
  void fetchMetar(const QString& icao);

  /**
   * Looks for the METAR.
   * \param icao Airport ICAO code. \note The code must be uppercase.
   * \return Pointer to the found METAR instance or _nullptr_ if not found.
   */
  const Metar* find(const QString& icao) const;
  
  /**
   * Returns the model index for the given metar.
   */
  const QModelIndex modelIndexForMetar(const Metar* metar) const;

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role) const;
  
  bool anyMetarsInQueue() const;

public slots:
  void updateAll();
  void clear();

private:
  void __addMetar(const QString& metar);
  bool __matches(const QString& word);
  
private slots:
  void __readMetar();
  
private:
  QQueue<QString>       __requests;
  QList<Metar>          __metarList;
  PlainTextDownloader*  __downloader;

};

#endif // METARLISTMODEL_H
