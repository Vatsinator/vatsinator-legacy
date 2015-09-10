/*
 * resourcemanager.cpp
 * Copyright (C) 2013-2014  Michał Garapich <michal@garapich.pl>
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

#include "network/dataupdater.h"
#include "network/filedownloader.h"
#include "network/plaintextdownloader.h"
#include "storage/filemanager.h"
#include "storage/settingsmanager.h"
#include "config.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "resourcemanager.h"

// start running after two seconds
static const int StartDelay = 2 * 1000;

// manifest file name, both on local storage and on the server
static const QString ManifestFileName = QStringLiteral("Manifest");

// how many days to have the database updated
static const int DaysToUpdate = 7;


ResourceManager::ResourceManager(QObject* parent) :
    QObject(parent)
{
    connect(this, &ResourceManager::vatsinatorVersionChecked, this, &ResourceManager::__checkDatabase);
    QTimer::singleShot(StartDelay, this, SLOT(__fetchVersion()));
}

ResourceManager::~ResourceManager() {}

void
ResourceManager::requestDatabaseSync()
{
    __syncDatabase();
}

bool
ResourceManager::__versionActual(const QString& version1, const QString& version2)
{
    auto ver1 = version1.split(QRegExp("\\D+"));
    auto ver2 = version2.split(QRegExp("\\D+"));
    
    for (int i = 0; i < ver1.size() && i < ver2.size(); ++i) {
        if (ver1[i].toInt() < ver2[i].toInt())
            return false;
            
        if (ver1[i].toInt() > ver2[i].toInt())
            return true;
    }
    
    return true;
}

void
ResourceManager::__fetchVersion()
{
    PlainTextDownloader* d = new PlainTextDownloader();
    connect(d, &PlainTextDownloader::finished, this, &ResourceManager::__parseVersion);
    d->fetch(QUrl(NetConfig::Vatsinator::repoUrl() % QStringLiteral("VERSION")));
}

void
ResourceManager::__parseVersion()
{
    PlainTextDownloader* downloader = qobject_cast<PlainTextDownloader*>(sender());
    Q_ASSERT(downloader);
    
    QString version = downloader->data();
    bool actual = __versionActual(QString(VATSINATOR_VERSION), version);
    
    qDebug("ResourceManager: version(%s) %s version(%s)",
           VATSINATOR_VERSION, actual ? ">=" : "<", qPrintable(version.simplified()));
           
    if (!actual)
        emit outdated();
    
    emit vatsinatorVersionChecked(actual ? Updated : Outdated);
    downloader->deleteLater();
}

void
ResourceManager::__checkDatabase(ResourceManager::VersionStatus status)
{
    if (status == ResourceManager::Outdated)
        emit databaseStatusChanged(CannotUpdate);
    
    QFile manifest(FileManager::path(ManifestFileName));
    
    qDebug("ResourceManager: Manifest file: %s", qPrintable(manifest.fileName()));
    
    if (manifest.open(QIODevice::ReadOnly)) {
        QDate today = QDate::currentDate();
        QDate when = QDate::fromString(manifest.readLine().simplified(), "yyyyMMdd");
        
        if (when.daysTo(today) < DaysToUpdate) {
            __lastUpdateDate = when;
            emit databaseStatusChanged(Updated);
        } else {
            emit databaseStatusChanged(Outdated);
            
            //       __syncDatabase();
            if (SM::get("network.database_integration").toBool())
                QTimer::singleShot(3000, this, SLOT(__syncDatabase()));
        }
        
        manifest.close();
        __lastUpdateDate = when;
    }
}

void
ResourceManager::__syncDatabase()
{
    emit databaseStatusChanged(Updating);
    
    DataUpdater* du = new DataUpdater();
    connect(du, &DataUpdater::updated, this, &ResourceManager::__databaseUpdated);
    connect(du, &DataUpdater::failed, this, &ResourceManager::__databaseFailed);
            
    du->update();
}

void
ResourceManager::__databaseUpdated()
{
    __checkDatabase(Updated);
    
    DataUpdater* d = qobject_cast<DataUpdater*>(sender());
    if (d)
        d->deleteLater();
}

void
ResourceManager::__databaseFailed()
{
    qCritical("ResourceManager: failed updating the database!");
    emit databaseStatusChanged(Outdated);
    
    DataUpdater* d = qobject_cast<DataUpdater*>(sender());
    if (d)
        d->deleteLater();
}


static void registerType()
{
    qRegisterMetaType<ResourceManager::VersionStatus>("ResourceManager::VersionStatus");
}
Q_COREAPP_STARTUP_FUNCTION(registerType)