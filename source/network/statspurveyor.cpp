/*
 * statspurveyor.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include <QtGui>
#include <QtNetwork>

#include "vatsinatorapplication.h"

#include "statspurveyor.h"
#include "defines.h"

// send the startup report after 10 seconds
static const int START_DELAY = 10 * 1000;

// request urls
static const QString STARTUP_PATH = "/startup.php?version=%1&os=%2";

static const QString OS_STRING =
#ifdef Q_OS_WIN32
  "Windows"
#elif defined Q_OS_LINUX
  "Linux"
#elif defined Q_OS_DARWIN
  "Darwin"
#else
  "unknown"
#endif
  ;

StatsPurveyor::StatsPurveyor(QObject* _parent) :
    QObject(_parent),
    __nam(this) {

  QTimer::singleShot(START_DELAY, this, SLOT(reportStartup()));
}

StatsPurveyor::~StatsPurveyor() {}

void
StatsPurveyor::reportStartup() {
  QString url = QString(VATSINATOR_STATS_URL) % STARTUP_PATH;
  QNetworkRequest request(url.arg(VATSINATOR_VERSION, OS_STRING));
  
  VatsinatorApplication::log("StatsPurveyor: startup request: %s",
                             qPrintable(request.url().toString()));
  
  __nam.get(request);
}

