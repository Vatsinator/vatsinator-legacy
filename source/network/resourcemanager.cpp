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
    QObject(parent) {
  
  connect(this, SIGNAL(vatsinatorVersionChecked(ResourceManager::VersionStatus)),
                SLOT(__checkDatabase(ResourceManager::VersionStatus)));
  
  QTimer::singleShot(StartDelay, this, SLOT(__fetchVersion()));
}

ResourceManager::~ResourceManager() {}

void
ResourceManager::requestDatabaseSync() {
  __syncDatabase();
}

bool
ResourceManager::__versionActual(const QString& version1, const QString& version2) {
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
ResourceManager::__fetchVersion() {
  PlainTextDownloader* fetcher = new PlainTextDownloader();
  
  connect(fetcher,    SIGNAL(finished(QString)),
          this,       SLOT(__parseVersion(QString)));
  connect(fetcher,    SIGNAL(finished(QString)),
          fetcher,    SLOT(deleteLater()));
  connect(fetcher,    SIGNAL(error()),
          fetcher,    SLOT(deleteLater()));
  
  fetcher->fetch(QUrl(NetConfig::Vatsinator::repoUrl() % QStringLiteral("VERSION")));
}

void
ResourceManager::__parseVersion(QString version) {
  bool actual = __versionActual(QString(VATSINATOR_VERSION), version);
  
  qDebug("ResourceManager: version(%s) %s version(%s)",
         VATSINATOR_VERSION, actual ? ">=" : "<", qPrintable(version.simplified()));
  
  if (!actual)
    emit outdated();
  
  emit vatsinatorVersionChecked(actual ? Updated : Outdated);
}    

void
ResourceManager::__checkDatabase(ResourceManager::VersionStatus status) {
  if (status == ResourceManager::Outdated) {
    emit databaseStatusChanged(CannotUpdate);
  }
  
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
ResourceManager::__syncDatabase() {
  emit databaseStatusChanged(Updating);
  
  DataUpdater* du = new DataUpdater();
  connect(du,   SIGNAL(updated()),
          this, SLOT(__databaseUpdated()));
  connect(du,   SIGNAL(updated()),
          du,   SLOT(deleteLater()));
  connect(du,   SIGNAL(failed()),
          this, SLOT(__databaseFailed()));
  connect(du,   SIGNAL(failed()),
          du,   SLOT(deleteLater()));
  
  du->update();
}

void
ResourceManager::__databaseUpdated() {
  __checkDatabase(Updated);
}

void
ResourceManager::__databaseFailed() {
  qCritical("ResourceManager: failed updating the database!");
  emit databaseStatusChanged(Outdated);
}
