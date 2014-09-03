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

#include <QtNetwork>

#include "storage/settingsmanager.h"
#include "ui/dialogs/letsendstatsdialog.h"
#include "ui/userinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "config.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "statspurveyor.h"

// send the startup report after 10 seconds
static constexpr int StartDelay = 10 * 1000;

// if stats query failed, retry in 1 minute
static constexpr int RetryDelay = 60 * 1000;

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
    __userDecision(NotYetMade),
    __nam(this),
    __reply(nullptr) {
  
  QSettings s;
  if (!s.contains("Decided/stats")) { // no decision made yet
    LetSendStatsDialog* dialog = new LetSendStatsDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,     SIGNAL(accepted()),
            this,       SLOT(__statsAccepted()));
    connect(dialog,     SIGNAL(rejected()),
            this,       SLOT(__statsRejected()));
    connect(vApp()->userInterface(),    SIGNAL(initialized()),
            dialog,                     SLOT(show()));
    QTimer::singleShot(StartDelay, this, SLOT(reportStartup()));
  } else {
    bool accepted = s.value("Settings/misc/send_statistics", false).toBool();
    if (accepted) {
      __userDecision = Accepted;
      QTimer::singleShot(StartDelay, this, SLOT(reportStartup()));
    } else {
      __userDecision = Declined;
    }
    
    connect(vApp()->settingsManager(),  SIGNAL(settingsChanged()),
            this,                       SLOT(__applySettings()));
  }
  
  connect(this, SIGNAL(newRequest()), SLOT(__nextIfFree()));
}

StatsPurveyor::~StatsPurveyor() {}

void 
StatsPurveyor::reportStartup() {
  static const QString StartupPath = "startup.php?version=%1&os=%2";
  QString url = QString(NetConfig::Vatsinator::statsUrl()) % StartupPath;
  QNetworkRequest request(url.arg(VATSINATOR_VERSION, osString()));
  
  __enqueueRequest(request);
}

void
StatsPurveyor::reportNoAtc(const QString& _atc) {
  static const QString NoAtcPath = "noatc.php?atc=%1";
  
  /* Discard no-atc reports before data is read */
  if (!vApp()->vatsimDataHandler()->isInitialized())
    return;
  
  QString url = QString(NetConfig::Vatsinator::statsUrl()) % NoAtcPath;
  QNetworkRequest request(url.arg(_atc));
  
  __enqueueRequest(request);
}

void
StatsPurveyor::__enqueueRequest(const QNetworkRequest& _request) {
  if (__userDecision == Declined)
    return;
  
  __requests.enqueue(_request);
  emit newRequest();
}

void
StatsPurveyor::__parseResponse() {
  QJsonParseError error;
  QJsonDocument document = QJsonDocument::fromJson(__reply->readAll(), &error);
  
  if (error.error == QJsonParseError::NoError) {
    QJsonObject root = document.object();
    int result = root["result"].toInt();
    if (result > 0) {
      __requests.dequeue();
      __reply->deleteLater();
      __reply = nullptr;
    } else {
      Q_ASSERT_X(false, "StatsPurveyor", "Invalid query");
    }
    
    if (!__requests.isEmpty())
        __nextRequest(); 
  } else {
    qWarning("StatsPurveyor: query failed (%s); retry in 1 minute...", qPrintable(error.errorString()));
    QTimer::singleShot(RetryDelay, this, SLOT(__nextRequest()));
    __reply->deleteLater();
    __reply = nullptr;
  }
}

void
StatsPurveyor::__nextRequest() {
  Q_ASSERT(!__requests.isEmpty());
  
  if (__userDecision == NotYetMade)
    return;
  
  qDebug("StatsPurveyor: request: %s", qPrintable(__requests.head().url().toString()));
  
  __reply = __nam.get(__requests.head());
  connect(__reply,      SIGNAL(finished()),
          this,         SLOT(__parseResponse()));
}

void
StatsPurveyor::__applySettings() {
  bool sendStats = SM::get("misc.send_statistics").toBool();
  if (sendStats) {
    __userDecision = Accepted;
    if (!__requests.isEmpty())
      __nextRequest();
  } else {
    __userDecision = Declined;
  }
}

void
StatsPurveyor::__statsAccepted() {
  __userDecision = Accepted;
  QSettings s;
  s.setValue("Decided/stats", true);
  s.setValue("Settings/misc/send_statistics", true);
  
  SM::updateUi("misc");
  
  connect(vApp()->settingsManager(),    SIGNAL(settingsChanged()),
          this,                         SLOT(__applySettings()));
  
  if (!__requests.isEmpty())
      __nextRequest();
}

void
StatsPurveyor::__statsRejected() {
  __userDecision = Declined;
  QSettings s;
  s.setValue("Decided/stats", true);
  s.setValue("Settings/misc/send_statistics", false);
  
  SM::updateUi("misc");
  
  connect(vApp()->settingsManager(),    SIGNAL(settingsChanged()),
          this,                         SLOT(__applySettings()));
}

void
StatsPurveyor::__nextIfFree() {
  if (!__reply)
    __nextRequest();
}
