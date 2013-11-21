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

#include "vatsinatorapplication.h"

#include "storage/filemanager.h"
#include "defines.h"

static const QString LocalDataLocation =
    QDesktopServices::storageLocation(QDesktopServices::DataLocation) % "Vatsinator/";


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

QString
FileManager::staticPath(FileManager::StaticDir _d) {
  switch (_d) {
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
FileManager::path(const QString& _f, bool _localOnly) {
  if (_localOnly)
    return LocalDataLocation % _f;
  
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

QByteArray
FileManager::md5Hash(const QString& _fname) {
  QFile file(_fname);
  
  if (!file.open(QIODevice::ReadOnly)) {
    return QByteArray();
  }
  
  return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
}

QByteArray
FileManager::md5Hash(QIODevice& _dev) {
  Q_ASSERT(_dev.isOpen());
  
  return QCryptographicHash::hash(_dev.readAll(), QCryptographicHash::Md5).toHex();
}

FileManager::FileHash::FileHash(const QByteArray& _md5) :
    md5(_md5) {}
