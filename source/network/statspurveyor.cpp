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
#include <qjson/parser.h>

#include "vatsinatorapplication.h"

#include "statspurveyor.h"
#include "defines.h"

// send the startup report after 10 seconds
static const int START_DELAY = 10 * 1000;

// if stats query failed, retry in 1 minute
static const int RETRY_DELAY = 60 * 1000;

// request urls
static const QString STARTUP_PATH = "/startup.php?version=%1&os=%2";
static const QString NOATC_PATH = "/noatc.php?atc=%1";

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
    __nam(this),
    __reply(nullptr) {

  QTimer::singleShot(START_DELAY, this, SLOT(reportStartup()));
}

StatsPurveyor::~StatsPurveyor() {}

void
StatsPurveyor::reportStartup() {
  QString url = QString(VATSINATOR_STATS_URL) % STARTUP_PATH;
  QNetworkRequest request(url.arg(VATSINATOR_VERSION, OS_STRING));
  
  __enqueueRequest(request);
}

void
StatsPurveyor::reportNoAtc(const QString& _atc) {
  QString url = QString(VATSINATOR_STATS_URL) % NOATC_PATH;
  QNetworkRequest request(url.arg(_atc));
  
  __enqueueRequest(request);
}

void
StatsPurveyor::__parseResponse() {
  QJson::Parser parser;
  bool ok;
  QVariantMap content = parser.parse(__reply, &ok).toMap();
  if (ok && __reply->error() == QNetworkReply::NoError) {
    int result = content["result"].toInt();
    if (result > 0) {
      __requests.dequeue();
      if (!__requests.isEmpty()) {
        __reply->deleteLater();
        __reply = nullptr;
        __nextRequest(); 
      }
    } else {
      Q_ASSERT_X(false, "StatsPurveyor", "Invalid query");
    }
  } else {
    VatsinatorApplication::log("StatsPurveyor: query failed; retry in 1 minute...");
    QTimer::singleShot(RETRY_DELAY, this, SLOT(__nextRequest()));
    __reply->deleteLater();
    __reply = nullptr;
  }
}

void
StatsPurveyor::__nextRequest() {
  Q_ASSERT(!__requests.isEmpty());
  Q_ASSERT(!__reply);
  
  VatsinatorApplication::log("StatsPurveyor: request: %s", qPrintable(__requests.head().url().toString()));
  
  __reply = __nam.get(__requests.head());
  connect(__reply,      SIGNAL(finished()),
          this,         SLOT(__parseResponse()));
}

void
StatsPurveyor::__enqueueRequest(const QNetworkRequest& _request) {
  __requests.enqueue(_request);
  
  if (!__reply)
    __nextRequest();
}
