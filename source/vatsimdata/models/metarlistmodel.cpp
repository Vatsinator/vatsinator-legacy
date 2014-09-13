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


MetarListModel::MetarListModel(PlainTextDownloader* _ptd, QObject* _parent) :
    QAbstractListModel(_parent),
    __downloader(_ptd) {
  connect(__downloader,         SIGNAL(finished(QString)),
          this,                 SLOT(__readMetar(QString)));
}

void
MetarListModel::fetchMetar(const QString& _icao) {
  __downloader->fetchData(vApp()->vatsimDataHandler()->metarUrl() + "?id=" + _icao.toLower());
  __requests.enqueue(_icao.simplified());
}

const Metar *
MetarListModel::find(const QString& _key) const {
  for (const Metar& m: __metarList)
    if (m.icao() == _key)
      return &m;

  return nullptr;
}

const QModelIndex
MetarListModel::modelIndexForMetar(const Metar* _m) const {
  for (int i = 0; i < __metarList.size(); ++i) {
    if (&__metarList.at(i) == _m)
      return createIndex(i, 0);
  }
  
  return QModelIndex();
}

int
MetarListModel::rowCount(const QModelIndex&) const {
  return __metarList.count();
}

QVariant
MetarListModel::data(const QModelIndex& _index, int _role) const {
  if (!_index.isValid() || _index.row() >= __metarList.size())
    return QVariant();

  switch (_role) {
    case Qt::DisplayRole:
      return __metarList.at(_index.row()).metar();
    case Qt::ToolTipRole:
      return __metarList.at(_index.row()).lastFetchTime();
    default:
      return QVariant();
  }
}

bool
MetarListModel::anyMetarsInQueue() const {
  return __downloader->anyTasksLeft();
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
MetarListModel::__addMetar(const QString& _metar) {
  for (Metar & m: __metarList) {
    if (m.icao() == _metar.left(4)) {
      m.setMetar(_metar);
      return;
    }
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __metarList.push_back(Metar(_metar.left(4), _metar));
  endInsertRows();
}

bool
MetarListModel::__matches(const QString& _word) {
  return (_word.length() == 4) &&
         (_word.startsWith(__requests.head(), Qt::CaseInsensitive));
}

void
MetarListModel::__readMetar(const QString& _metar) {
  QString metar = _metar.simplified();

  if (metar.isEmpty())
    return;

  if (metar.contains(NoMetarText)) {
    emit noMetar(__requests.dequeue());
    return;
  }

  QString oneMetar;

  for (const QString& word: metar.split(' ')) {
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
