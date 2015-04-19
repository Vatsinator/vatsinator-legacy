/*
 * metarupdater.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include <QtCore>

#include "network/plaintextdownloader.h"
#include "ui/models/metarlistmodel.h"
#include "ui/models/roles.h"
#include "vatsimdata/metar.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "metarupdater.h"

static Q_CONSTEXPR int UpdateInterval = 5 * 1000 * 60; // 5 minutes

MetarUpdater::MetarUpdater (MetarListModel* model, QObject* parent) :
    QObject (parent),
    __metars(model),
    __downloader(new PlainTextDownloader(this)) {
  connect(__downloader, &PlainTextDownloader::finished, this, &MetarUpdater::__readMetars);
  startTimer(UpdateInterval, Qt::VeryCoarseTimer);
}

void
MetarUpdater::fetch(QString icao) {
  QString args = QStringLiteral("?id=%1").arg(icao.toLower());
  QUrl url(vApp()->vatsimDataHandler()->metar().toString() % args);
  __downloader->fetch(url);
  __requests.enqueue(icao);
}

void
MetarUpdater::update() {
  for (int i = 0; i < __metars->rowCount(); ++i) {
    auto index = __metars->data(__metars->index(i), MetarRole);
    if (index.isValid()) {
      Q_ASSERT(index.canConvert<Metar>());
      Metar m = index.value<Metar>();
      fetch(m.icao());
    }
  }
}

void
MetarUpdater::timerEvent(QTimerEvent* event) {
  update();
  Q_UNUSED(event);
}

void
MetarUpdater::__readMetars() {
  // How to recognize unavailable metars.
  // This value represents the response returned by vatsim server
  // in case given METAR could not be found.
  static const QString NoMetarText = QStringLiteral("No METAR available");
  
  QString response = __downloader->data().simplified();

  if (response.isEmpty())
    return;

  if (response.contains(NoMetarText)) {
    __requests.dequeue();
    return;
  }
  
  QString metar;
  QStringList words = response.split(' ');
  for (const auto& w: words) {
    if (w.length() == 4 && w.startsWith(__requests.head(), Qt::CaseInsensitive)) {
      if (!metar.isEmpty())
        __metars->addOrUpdate(metar);
      
      metar.clear();
    }

    metar.append(w).append(" ");
  }

  if (!metar.isEmpty())
    __metars->addOrUpdate(metar);

  __requests.dequeue();
}


