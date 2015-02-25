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

#include <QtCore>

#include "storage/cachefile.h"
#include "config.h"
#include "vatsinatorapplication.h"

#include "storage/filemanager.h"

static const QString LocalDataLocation =
    QDir::cleanPath(
        QStandardPaths::writableLocation(QStandardPaths::DataLocation)
      % QDir::separator()
      % QStringLiteral("Vatsinator")
    ) % QDir::separator();


FileManager::FileManager() {
  qDebug("FileManager: local data location: %s", qPrintable(LocalDataLocation));
  
  // ensure that our data directory exists
  QDir dir(LocalDataLocation);
  if (!dir.exists()) {
    qDebug("FileManager: creating directory %s", qPrintable(LocalDataLocation));
    dir.mkpath(".");
  }
}

void
FileManager::cacheData(const QString& fileName, const QString& data) {
  cacheData(fileName, data.toUtf8());
}

void
FileManager::cacheData(const QString& fileName, const QByteArray& data) {
  CacheFile cache(fileName);
  cache.open(QIODevice::WriteOnly | QIODevice::Truncate);
  cache.write(data);
  cache.close();
}

bool
FileManager::moveToCache(const QString& source, const QString& destination) {
  QFile file(source);
  if (!file.open(QIODevice::ReadWrite)) {
    qWarning("FileManager: failed to access file %s", qPrintable(source));
    return false;
  }
  
  file.close();
  
  CacheFile oldCache(destination);
  QFileInfo info(oldCache);
  QDir(info.path()).mkpath(".");
  
  if (oldCache.exists())
    oldCache.remove();
  
  bool result = file.rename(oldCache.fileName());
  if (result)
    qDebug("FileManager: cached file %s", qPrintable(destination));
  else
    qWarning("FileManager: failed caching file %s", qPrintable(destination));
  
  return result;
}

QString
FileManager::staticPath(FileManager::StaticDir directory) {
  switch (directory) {
    case Plugins:
#ifndef Q_OS_DARWIN
      return QStringLiteral(VATSINATOR_PREFIX) % QStringLiteral("plugins");
#else
      return QCoreApplication::applicationDirPath() % QStringLiteral("../Resources/plugins");
#endif
      
    case Translations:
#ifndef Q_OS_DARWIN
      return QStringLiteral(VATSINATOR_PREFIX) % QStringLiteral("translations");
#else
      return QCoreApplication::applicationDirPath() % QStringLiteral("/../Resources/translations");
#endif
    
    default:
      Q_UNREACHABLE();
  }
}

QString
FileManager::path(const QString& fileName) {
  
  QFile tryLocal(LocalDataLocation % fileName);
  if (tryLocal.exists()) {
    qDebug("FileManager: file %s loaded from %s.",
           qPrintable(fileName), qPrintable(tryLocal.fileName()));
    return tryLocal.fileName();
  } else {
    return
#ifndef Q_OS_DARWIN
      QStringLiteral(VATSINATOR_PREFIX)
#else // on MacOS look for the file in the bundle
      QCoreApplication::applicationDirPath() % QStringLiteral("/../Resources/")
#endif
      % fileName;
  }
}

QString
FileManager::localDataPath() {
  return LocalDataLocation;
}
