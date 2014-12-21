/*
    metarlistmodel.cpp
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

#include <QtCore>

#include "network/plaintextdownloader.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "metarlistmodel.h"

// How to recognize unavailable metars.
// This value represents the response returned by vatsim server
// in case given METAR could not be found.
static const QString NoMetarText = QStringLiteral("No METAR available");


MetarListModel::MetarListModel(PlainTextDownloader* downloader, QObject* parent) :
    QAbstractListModel(parent),
    __downloader(downloader) {
  connect(__downloader,         SIGNAL(finished()),
          this,                 SLOT(__readMetar()));
}

void
MetarListModel::fetchMetar(const QString& icao) {
  __downloader->fetch(vApp()->vatsimDataHandler()->metar().toString() + "?id=" + icao.toLower());
  __requests.enqueue(icao.simplified());
}

const Metar*
MetarListModel::find(const QString& icao) const {
  for (const Metar& m: __metarList)
    if (m.icao() == icao)
      return &m;

  return nullptr;
}

const QModelIndex
MetarListModel::modelIndexForMetar(const Metar* metar) const {
  for (int i = 0; i < __metarList.size(); ++i) {
    if (&__metarList.at(i) == metar)
      return createIndex(i, 0);
  }
  
  return QModelIndex();
}

int
MetarListModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
  return __metarList.count();
}

QVariant
MetarListModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= __metarList.size())
    return QVariant();

  switch (role) {
    case Qt::DisplayRole:
      return __metarList.at(index.row()).metar();
    case Qt::ToolTipRole:
      return __metarList.at(index.row()).lastFetchTime();
    default:
      return QVariant();
  }
}

bool
MetarListModel::anyMetarsInQueue() const {
  return __downloader->hasPendingTasks();
}

void
MetarListModel::updateAll() {
  for (Metar & m: __metarList)
    fetchMetar(m.icao());
}

void
MetarListModel::clear() {
  beginRemoveRows(QModelIndex(), 0, __metarList.size() - 1);
  __metarList.clear();
  endRemoveRows();
  emit newMetarsAvailable();
}

void
MetarListModel::__addMetar(const QString& metar) {
  for (Metar & m: __metarList) {
    if (m.icao() == metar.left(4)) {
      m.setMetar(metar);
      return;
    }
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __metarList.push_back(Metar(metar.left(4), metar));
  endInsertRows();
}

bool
MetarListModel::__matches(const QString& word) {
  return (word.length() == 4) &&
         (word.startsWith(__requests.head(), Qt::CaseInsensitive));
}

void
MetarListModel::__readMetar() {
  QString cleanMetar = __downloader->data().simplified();

  if (cleanMetar.isEmpty())
    return;

  if (cleanMetar.contains(NoMetarText)) {
    emit noMetar(__requests.dequeue());
    return;
  }

  QString oneMetar;
  for (const QString& word: cleanMetar.split(' ')) {
    if (__matches(word)) {
      if (!oneMetar.isEmpty())
        __addMetar(oneMetar);

      oneMetar.clear();
    }

    oneMetar.append(word % " ");
  }

  if (!oneMetar.isEmpty())
    __addMetar(oneMetar);

  __requests.dequeue();
  emit newMetarsAvailable();
}
