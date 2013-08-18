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

static const QString DATA_DIR_LOCATON(
#ifdef Q_OS_WIN32
  QDir::fromNativeSeparators(qgetenv("LOCALAPPDATA"))
#elif defined Q_OS_LINUX
  QDir::homePath() % "/.local/share"
#elif defined Q_OS_DARWIN
  QDir::homePath() % "/Library/Application Support"
#endif
);

static const QString DATA_LOCATION(QDir::toNativeSeparators(DATA_DIR_LOCATON % "/Vatsinator"));


FileManager::FileManager() {
  VatsinatorApplication::log("Local data location: %s", qPrintable(DATA_LOCATION));
  
  __readManifest(DATA_LOCATION % "/Manifest");
}

void
FileManager::cacheData(const QString& _fileName, const QString& _data) {
  CacheFile cache(_fileName);
  cache.open(QIODevice::WriteOnly | QIODevice::Truncate);
  cache.write(_data.toUtf8());
  cache.close();
}

QString
FileManager::path(const QString& _f) {
  QFile tryLocal(DATA_LOCATION % _f);
  if (tryLocal.exists()) {
    VatsinatorApplication::log("File %s loaded from %s.",
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


void
FileManager::__readManifest(const QString& _fname) {
  QFile file(_fname);
  
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }
  
  while (!file.atEnd()) {
    QByteArray line = file.readLine().simplified();
    
    if (line.isEmpty())
      continue;
    
    if (!__manifest.timestamp.isValid()) {
      __manifest.timestamp = QDateTime::fromString(QString::fromUtf8(line), "yyyyMMddhhmm");
    } else {
      QList<QByteArray> split = line.split(' ');
      
      if (split.length() != 2)
        return;
      
      __manifest.hash.insert(QString::fromUtf8(split[0]), FileHash(split[1]));
    }
  }
  
  file.close();
  
  VatsinatorApplication::log("Data updated on %s.",
                             qPrintable(__manifest.timestamp.toString("yyyyMMddhhmm")));
}

