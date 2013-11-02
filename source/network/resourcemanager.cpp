/*
 * resourcemanager.cpp
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

#include "network/plaintextdownloader.h"

#include "storage/filemanager.h"
#include "storage/settingsmanager.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "resourcemanager.h"
#include "defines.h"

// start running after two seconds
static const int START_DELAY = 2 * 1000;

ResourceManager::ResourceManager(QObject* _parent) :
    QObject(_parent) {
  
  qRegisterMetaType<ResourceManager::VersionStatus>("ResourceManager::VersionStatus");
  
  QTimer::singleShot(START_DELAY, this, SLOT(__fetchVersion()));
}

ResourceManager::~ResourceManager() {}

void
ResourceManager::__fetchVersion() {
  if (SM::get("network.version_check").toBool()) {
    PlainTextDownloader* fetcher = new PlainTextDownloader();
    
    connect(fetcher,      SIGNAL(finished(QString)),
            this,         SLOT(__parseVersion(QString)));
    
    fetcher->fetchData(QString(NetConfig::Vatsinator::repoUrl()) % "/VERSION");
  }
}

void
ResourceManager::__parseVersion(QString _versionString) {
  bool actual = __versionActual(QString(VATSINATOR_VERSION), _versionString);
  
  VatsinatorApplication::log("ResourceManager: version(%1) %2 version(%3)",
                             VATSINATOR_VERSION,
                             actual ? ">=" : "<",
                             qPrintable(_versionString));
  
  if (!actual)
    emit outdated();
  
  emit versionChecked(actual ? Updated : Outdated);
  
  sender()->deleteLater();
}

bool
ResourceManager::__versionActual(const QString& _version1, const QString& _version2) {
  auto ver1 = _version1.split(QRegExp("\\D+"));
  auto ver2 = _version2.split(QRegExp("\\D+"));

  for (int i = 0; i < ver1.size() && i < ver2.size(); ++i) {
    if (ver1[i].toInt() < ver2[i].toInt())
      return false;
    
    if (ver1[i].toInt() > ver2[i].toInt())
      return true;
  }
  
  return true;
}
