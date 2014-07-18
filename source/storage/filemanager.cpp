/*
    filemanager.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>

#include "storage/cachefile.h"
#include "config.h"
#include "vatsinatorapplication.h"

#include "storage/filemanager.h"

static const QString LocalDataLocation =
    QDir::cleanPath(
        QDesktopServices::storageLocation(QDesktopServices::DataLocation)
      % QDir::separator()
      % "Vatsinator"
    ) % QDir::separator();


FileManager::FileManager() {
  VatsinatorApplication::log("FileManager: local data location: %s", qPrintable(LocalDataLocation));
  
  // ensure that our data directory exists
  QDir dir(LocalDataLocation);
  if (!dir.exists()) {
    VatsinatorApplication::log("FileManager: creating directory %s.", qPrintable(LocalDataLocation));
    dir.mkpath(".");
  }
}

void
FileManager::cacheData(const QString& _fileName, const QString& _data) {
  CacheFile cache(_fileName);
  cache.open(QIODevice::WriteOnly | QIODevice::Truncate);
  cache.write(_data.toUtf8());
  cache.close();
}

bool
FileManager::moveToCache(const QString& _source, const QString& _destination) {
  QFile file(_source);
  if (!file.open(QIODevice::ReadWrite)) {
    VatsinatorApplication::log("FileManager: failed to access file %s", qPrintable(_source));
    return false;
  }
  
  file.close();
  
  CacheFile oldCache(_destination);
  QFileInfo info(oldCache);
  QDir(info.path()).mkpath(".");
  
  if (oldCache.exists())
    oldCache.remove();
  
  bool result = file.rename(oldCache.fileName());
  if (result)
    VatsinatorApplication::log("FileManager: cached file %s", qPrintable(_destination));
  else
    VatsinatorApplication::log("FileManager: failed caching file %s", qPrintable(_destination));
  
  return result;
}

QString
FileManager::staticPath(FileManager::StaticDir _d) {
  switch (_d) {
    case Plugins:
#ifndef Q_OS_DARWIN
      return QString(VATSINATOR_PREFIX) % "plugins";
#else
      return QCoreApplication::applicationDirPath() % "../Resources/plugins";
#endif
    
    case Pixmaps:
#ifndef Q_OS_DARWIN
      return QString(VATSINATOR_PREFIX) % "pixmaps";
#else
      return QCoreApplication::applicationDirPath() % "/../Resources/pixmaps";
#endif
      
    case Translations:
#ifndef Q_OS_DARWIN
      return QString(VATSINATOR_PREFIX) % "translations";
#else
      return QCoreApplication::applicationDirPath() % "/../Resources/translations";
#endif
    
    default:
      Q_ASSERT_X(false, "getting static path", "No such file!");
      return QString();
  }
}

QString
FileManager::path(const QString& _f) {
  
  QFile tryLocal(LocalDataLocation % _f);
  if (tryLocal.exists()) {
    VatsinatorApplication::log("FileManager: file %s loaded from %s.",
                               qPrintable(_f),
                               qPrintable(tryLocal.fileName()));
    return tryLocal.fileName();
  } else {
    return
#ifndef Q_OS_DARWIN
      static_cast<QString>(VATSINATOR_PREFIX)
#else // on MacOS look for the file in the bundle
      QCoreApplication::applicationDirPath() % "/../Resources/"
#endif
      % _f;
  }
}

QString
FileManager::localDataPath() {
  return LocalDataLocation;
}
