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

#include "storage/settingsmanager.h"

#include "ui/dialogs/letsendstatsdialog.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "statspurveyor.h"
#include "defines.h"

// send the startup report after 10 seconds
static constexpr int StartDelay = 10 * 1000;

// if stats query failed, retry in 1 minute
static constexpr int RetryDelay = 60 * 1000;

// request urls
static const QString STARTUP_PATH = "startup.php?version=%1&os=%2";
static const QString NOATC_PATH = "noatc.php?atc=%1";

namespace {
  
  /**
   * Gets the string representing the running operating system.
   */
  QString osString() {
#ifdef Q_OS_WIN32
    switch (QSysInfo::WindowsVersion) {
      case QSysInfo::WV_32s:
        return "Windows 3.1";
      case QSysInfo::WV_95:
        return "Windows 95";
      case QSysInfo::WV_98:
        return "Windows 98";
      case QSysInfo::WV_NT:
        return "Windows NT";
      case QSysInfo::WV_2000:
        return "Windows 2000";
      case QSysInfo::WV_XP:
        return "Windows XP";
      case QSysInfo::WV_2003:
        return "Windows XP Professional x64";
      case QSysInfo::WV_VISTA:
        return "Windows Vista";
      case QSysInfo::WV_WINDOWS7:
        return "Windows 7";
      default:
        return "Windows";
    }
#elif defined Q_OS_LINUX
    /* On Linux we have to get distro name somehow, lets try lsb-release */
    QProcess p;
    p.start("/usr/bin/lsb_release", {"--description", "--short"});
    if (p.error() == QProcess::UnknownError) { // no error
      p.waitForFinished(-1);
      QString d = p.readAllStandardOutput();
      d.remove("\"");
      d = d.simplified();
      
      return QString("Linux ") % d;
    } else {
      return QString("Linux");
    }
#elif defined Q_OS_DARWIN
    switch (QSysInfo::MacintoshVersion) {
      case QSysInfo::MV_10_3:
        return "Mac OS X 10.3";
      case QSysInfo::MV_10_4:
        return "Mac OS X 10.4";
      case QSysInfo::MV_10_5:
        return "Mac OS X 10.5";
      case QSysInfo::MV_10_6:
        return "Mac OS X 10.6";
      case QSysInfo::MV_10_7:
        return "Mac OS X 10.7";
      case QSysInfo::MV_10_8:
        return "Mac OS X 10.8";
      default:
        return "Mac OS X";
    }
#else
# warning "Define operating system string here"
    return "Unknown";
#endif
  }
  
}
  

StatsPurveyor::StatsPurveyor(QObject* _parent) :
    QObject(_parent),
    __nam(this),
    __reply(nullptr) {
      
 /* Do not report stats until we read config file or user makes the decision */
  __nam.setNetworkAccessible(QNetworkAccessManager::NotAccessible);
  
  QSettings s;
  if (!s.contains("Decided/stats")) {
    LetSendStatsDialog* dialog = new LetSendStatsDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,     SIGNAL(accepted()),
            this,       SLOT(__statsAccepted()));
    connect(dialog,     SIGNAL(rejected()),
            this,       SLOT(__statsRejected()));
    connect(VatsinatorApplication::getSingletonPtr(),   SIGNAL(uiCreated()),
            dialog,                                     SLOT(show()));
  } else {
    /* In the meantime, settingsChanged() signal will be emited, so user's
     * preferences will be honored. */
    QTimer::singleShot(StartDelay, this, SLOT(reportStartup()));
  }
  
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__applySettings()));
  connect(this,                                 SIGNAL(newRequest()),
          this,                                 SLOT(__nextIfFree()));
}

StatsPurveyor::~StatsPurveyor() {}

void
StatsPurveyor::reportStartup() {
  QString url = QString(NetConfig::Vatsinator::statsUrl()) % STARTUP_PATH;
  QNetworkRequest request(url.arg(VATSINATOR_VERSION, osString()));
  
  __enqueueRequest(request);
}

void
StatsPurveyor::reportNoAtc(const QString& _atc) {
  QString url = QString(NetConfig::Vatsinator::statsUrl()) % NOATC_PATH;
  QNetworkRequest request(url.arg(_atc));
  
  __enqueueRequest(request);
}

void
StatsPurveyor::__enqueueRequest(const QNetworkRequest& _request) {
  if (__nam.networkAccessible() == QNetworkAccessManager::NotAccessible)
    return;
  
  __requests.enqueue(_request);
  emit newRequest();
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
      __reply->deleteLater();
      __reply = nullptr;
      if (!__requests.isEmpty())
        __nextRequest(); 
    } else {
      Q_ASSERT_X(false, "StatsPurveyor", "Invalid query");
    }
  } else {
    VatsinatorApplication::log("StatsPurveyor: query failed; retry in 1 minute...");
    QTimer::singleShot(RetryDelay, this, SLOT(__nextRequest()));
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
StatsPurveyor::__applySettings() {
  bool sendStats = SM::get("misc.send_statistics").toBool();
  if (sendStats)
    __nam.setNetworkAccessible(QNetworkAccessManager::Accessible);
  else
    __nam.setNetworkAccessible(QNetworkAccessManager::NotAccessible);
}

void
StatsPurveyor::__statsAccepted() {
  QSettings s;
  s.setValue("Decided/stats", true);
  s.setValue("Settings/misc/send_statistics", true);
  QTimer::singleShot(StartDelay, this, SLOT(reportStartup()));
  
  SM::updateUi("misc");
}

void
StatsPurveyor::__statsRejected() {
  QSettings s;
  s.setValue("Decided/stats", true);
  s.setValue("Settings/misc/send_statistics", false);
  
  SM::updateUi("misc");
}

void
StatsPurveyor::__nextIfFree() {
  if (!__reply)
    __nextRequest();
}
