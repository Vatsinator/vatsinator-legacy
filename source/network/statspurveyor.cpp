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
static const int StartDelay = 10 * 1000;

// if stats query failed, retry in 1 minute
static const int RetryDelay = 60 * 1000;

namespace {

/**
 * Gets the string representing the running operating system.
 */
QString osString()
{
#ifdef Q_OS_WIN32

    switch (QSysInfo::WindowsVersion) {
        case QSysInfo::WV_32s:
            return QStringLiteral("Windows 3.1");
            
        case QSysInfo::WV_95:
            return QStringLiteral("Windows 95");
            
        case QSysInfo::WV_98:
            return QStringLiteral("Windows 98");
            
        case QSysInfo::WV_NT:
            return QStringLiteral("Windows NT");
            
        case QSysInfo::WV_2000:
            return QStringLiteral("Windows 2000");
            
        case QSysInfo::WV_XP:
            return QStringLiteral("Windows XP");
            
        case QSysInfo::WV_2003:
            return QStringLiteral("Windows XP Professional x64");
            
        case QSysInfo::WV_VISTA:
            return QStringLiteral("Windows Vista");
            
        case QSysInfo::WV_WINDOWS7:
            return QStringLiteral("Windows 7");
            
        case QSysInfo::WV_WINDOWS8:
            return QStringLiteral("Windows 8");
            
        case QSysInfo::WV_WINDOWS8_1:
            return QStringLiteral("Windows 8.1");
            
        default:
            return QStringLiteral("Windows");
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
    
        return QStringLiteral("Linux ") % d;
    } else
        return QStringLiteral("Linux");
    
#elif defined Q_OS_MAC
    
    switch (QSysInfo::MacintoshVersion) {
        case QSysInfo::MV_10_3:
            return QStringLiteral("Mac OS X 10.3");
    
        case QSysInfo::MV_10_4:
            return QStringLiteral("Mac OS X 10.4");
    
        case QSysInfo::MV_10_5:
            return QStringLiteral("Mac OS X 10.5");
    
        case QSysInfo::MV_10_6:
            return QStringLiteral("Mac OS X 10.6");
    
        case QSysInfo::MV_10_7:
            return QStringLiteral("Mac OS X 10.7");
    
        case QSysInfo::MV_10_8:
            return QStringLiteral("Mac OS X 10.8");
    
        case QSysInfo::MV_10_9:
            return QStringLiteral("Mac OS X 10.9");
    
        default:
            return "Mac OS X";
    }
    
#else
# warning "Define operating system string here"
    return "Unknown";
#endif
}

}


StatsPurveyor::StatsPurveyor(QObject* parent) :
    QObject(parent),
    __userDecision(NotYetMade),
    __nam(this),
    __reply(nullptr)
{

    QSettings s;
    
    if (s.contains("Decided/stats")) {
        bool accepted = s.value("Settings/misc/send_statistics", false).toBool();
        
        if (accepted) {
            __userDecision = Accepted;
            QTimer::singleShot(StartDelay, Qt::VeryCoarseTimer, this, SLOT(reportStartup()));
        } else
            __userDecision = Declined;
    }
    
    connect(this, &StatsPurveyor::newRequest, this, &StatsPurveyor::__nextIfFree);
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged,
            this, &StatsPurveyor::__applySettings);
}

StatsPurveyor::~StatsPurveyor() {}

void
StatsPurveyor::setUserDecision(StatsPurveyor::UserDecision decision)
{
    __userDecision = decision;
    
    QSettings s;
    s.setValue("Decided/stats", true);
    s.setValue("Settings/misc/send_statistics", __userDecision == Accepted);
    SM::updateUi("misc");
    
    if (!__requests.isEmpty()) {
        if (__userDecision == Accepted)
            __nextIfFree();
        else if (__userDecision == Declined)
            __requests.clear();
    }
}

void
StatsPurveyor::reportStartup()
{
    static const QString StartupPath = QStringLiteral("startup.php?version=%1&os=%2");
    QString url = NetConfig::Vatsinator::statsUrl() % StartupPath;
    QNetworkRequest request(url.arg(VATSINATOR_VERSION, osString()));
    
    __enqueueRequest(request);
}

void
StatsPurveyor::reportNoAtc(const QString& callsign)
{
    static const QString NoAtcPath = QStringLiteral("noatc.php?atc=%1");
    
    /* Discard no-atc reports before data is read */
    if (!vApp()->vatsimDataHandler()->isInitialized())
        return;
        
    QString url = NetConfig::Vatsinator::statsUrl() % NoAtcPath;
    QNetworkRequest request(url.arg(callsign));
    
    __enqueueRequest(request);
}

void
StatsPurveyor::__enqueueRequest(const QNetworkRequest& request)
{
    if (__userDecision == Declined)
        return;
        
    __requests.enqueue(request);
    emit newRequest();
}

void
StatsPurveyor::__parseResponse()
{
    QJsonParseError error;
    QByteArray data = __reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data, &error);
    
    if (error.error == QJsonParseError::NoError) {
        QJsonObject root = document.object();
        int result = root["result"].toInt();
        
        if (result > 0) {
            __requests.dequeue();
            __reply->deleteLater();
            __reply = nullptr;
        } else {
            qFatal("Invalid query response; query: %s, response: %s",
                   qPrintable(__reply->request().url().toString()), data.constData());
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
StatsPurveyor::__nextRequest()
{
    Q_ASSERT(!__requests.isEmpty());
    
    if (__userDecision == NotYetMade)
        return;
        
    qDebug("StatsPurveyor: request: %s", qPrintable(__requests.head().url().toString()));
    
    QNetworkRequest request = __requests.head();
    __reply = __nam.get(request);
    connect(__reply, &QNetworkReply::finished, this, &StatsPurveyor::__parseResponse);
}

void
StatsPurveyor::__applySettings()
{
    bool sendStats = SM::get("misc.send_statistics").toBool();
    
    if (sendStats) {
        __userDecision = Accepted;
        
        if (!__requests.isEmpty())
            __nextRequest();
    } else
        __userDecision = Declined;
}

void
StatsPurveyor::__nextIfFree()
{
    if (!__reply)
        __nextRequest();
}
