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

#include "vatsinatorapplication.h"

#include "resourcemanager.h"
#include "defines.h"

// start running after two seconds
static const int START_DELAY = 2 * 1000;

ResourceManager::ResourceManager(QObject* _parent) :
    QObject(_parent),
    __currentVersion(VATSINATOR_VERSION) {
  
  qRegisterMetaType<ResourceManager::VersionStatus>("ResourceManager::VersionStatus");
  
  QTimer::singleShot(START_DELAY, this, SLOT(__fetchVersion()));
}

ResourceManager::~ResourceManager() {}

void
ResourceManager::__fetchVersion() {
  PlainTextDownloader* fetcher = new PlainTextDownloader();
  
  connect(fetcher,      SIGNAL(finished(QString)),
          this,         SLOT(__parseVersion(QString)));
  
  fetcher->fetchData(QString(VATSINATOR_REPO_URL) % "/VERSION");
}

void
ResourceManager::__parseVersion(QString _versionString) {
  Version latest(_versionString);
  
  if (__currentVersion < latest)
    emit outdated();
  
  emit versionChecked(__currentVersion < latest ? Outdated : Updated);
  
  sender()->deleteLater();
}

ResourceManager::Version::Version(const QString& _str) {
  __major = _str.section('.', 0, 0).toInt();
  
  if (_str.contains('-')) {
    __minor = _str.section('.', 1).section('-', 0, 0).toInt();
  } else {
    __minor = _str.section('.', 1).toInt();
  }
  
  VatsinatorApplication::log("Parsed %s as major=%i, minor=%i",
                             qPrintable(_str), __major, __minor);
}

ResourceManager::Version::Version() :
  __major(-1),
  __minor(-1) {}

bool
ResourceManager::Version::operator<(const ResourceManager::Version& _other) {
if (__major < _other.__major)
    return true;
  
  if (__major == _other.__major && __minor < _other.__minor)
    return true;
  
  return false;
}
