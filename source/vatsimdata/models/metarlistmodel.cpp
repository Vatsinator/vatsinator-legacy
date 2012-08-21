/*
    metarlistmodel.cpp
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

#include <QtGui>

#include "network/httphandler.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "metarlistmodel.h"
#include "defines.h"

MetarListModel::MetarListModel(HttpHandler* _hh, QObject* _parent) :
    QAbstractListModel(_parent),
    __myHttpHandler(_hh) {
  connect(__myHttpHandler,  SIGNAL(finished(QString)),
          this,     SLOT(__gotMetar(QString)));
  connect(VatsinatorApplication::GetSingletonPtr(), SIGNAL(metarsRefreshRequested()),
          this,     SLOT(updateAllMetars()));
}

void
MetarListModel::fetchMetar(const QString& _icao) {
  __myHttpHandler->fetchData(VatsimDataHandler::GetSingleton().getMetarUrl() + "?id=" + _icao.toLower());
  __requests.enqueue(_icao.simplified());
}

const Metar*
MetarListModel::find(const QString& _key) const {
  for (const Metar & m: __metarList)
    if (m.getIcao() == _key)
      return &m;

  return static_cast< const Metar* >(NULL);
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
      return __metarList.at(_index.row()).getMetar();
    case Qt::ToolTipRole:
      return __metarList.at(_index.row()).getLastFetchedTime();
    default:
      return QVariant();
  }
}

bool
MetarListModel::anyMetarsInQueue() const {
  return __myHttpHandler->anyTasksLeft();
}

void
MetarListModel::updateAllMetars() {
  for (Metar & m: __metarList)
    fetchMetar(m.getIcao());
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
    if (m.getIcao() == _metar.left(4)) {
      m.setMetar(_metar);
      return;
    }
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  __metarList.push_back(Metar(_metar.left(4), _metar));
  endInsertRows();
}

inline bool
MetarListModel::__matches(const QString& _word) {
  return (_word.length() == 4) &&
         (_word.startsWith(__requests.head(), Qt::CaseInsensitive));
}

void
MetarListModel::__gotMetar(const QString& _metar) {
  QString metar = _metar.simplified();

  if (metar.isEmpty())
    return;

  if (metar.contains(METAR_NO_AVAIL)) {
    emit noMetar(__requests.dequeue());
    return;
  }

  QString oneMetar;

for (QString & word: metar.split(' ')) {
    if (__matches(word)) {
      if (!oneMetar.isEmpty())
        __addMetar(oneMetar);

      oneMetar.clear();
    }

    oneMetar.append(word + " ");
  }

  if (!oneMetar.isEmpty())
    __addMetar(oneMetar);

  __requests.dequeue();

  emit newMetarsAvailable();
}


