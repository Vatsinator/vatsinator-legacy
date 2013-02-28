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

#include "network/filedownloader.h"

#include "vatsinatorapplication.h"

#include "filemanager.h"
#include "defines.h"

static const QString DATA_DIR_LOCATON(
#ifdef Q_OS_WIN32
  QDir::fromNativeSeparators(qgetenv("LOCALAPPDATA"))
#elif defined Q_OS_LINUX
  QDir::homePath() % "/.local/share"
#elif defined Q_OS_DARWIN
  // TODO: fill it in
#endif
);

static const QString DATA_LOCATION(QDir::toNativeSeparators(DATA_DIR_LOCATON % "/Vatsinator/"));


FileManager::FileManager() :
    __downloader(new FileDownloader()) {
  VatsinatorApplication::log("Local data location: %s", qPrintable(DATA_LOCATION));
  
  __readManifest(DATA_LOCATION % "Manifest");
}

FileManager::~FileManager() {
  delete __downloader;
}

const QString &
FileManager::path(FileManager::File _f) {
  if (FileManager::getSingleton().__files[_f].isEmpty())
    FileManager::getSingleton().__findFile(_f);
  
  return FileManager::getSingleton().__files[_f];
}

QString
FileManager::enum2Str(FileManager::File _f) {
  switch (_f) {
    case AIRPORT_DB:
      return "WorldAirports.db";
    
    case FIR_DB:
      return "WorldFirs.db";
      
    case WORLD_DB:
      return "WorldMap.db";
      
    case ALIAS:
      return "data/alias";
      
    case COUNTRY:
      return "data/country";
    
    case FIR:
      return "data/fir";
      
    case MODEL:
      return "data/model";
      
    case UIR:
      return "data/uir";
      
    default:
      Q_ASSERT(false);
      break;
  }
  
  return QString();
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
      __manifest.timestamp = QDateTime::fromString(QString::fromUtf8(line), "yyyyMMddhhmmss");
    } else {
      QList< QByteArray > split = line.split(' ');
      
      if (split.length() != 2)
        return;
      
      __manifest.hash.insert(QString::fromUtf8(split[0]), FileHash(split[1]));
    }
  }
  
  file.close();
  
  VatsinatorApplication::log("Data updated on %s.",
                             qPrintable(__manifest.timestamp.toString("yyyyMMddhhmmss")));
}

void
FileManager::__findFile(FileManager::File _f) {
  QString fname = enum2Str(_f);
  
  QFile tryLocal(DATA_LOCATION % fname);
  if (tryLocal.exists()) {
    if (md5Hash(tryLocal.fileName()) == __manifest.hash[fname].md5) {
      __files[_f] = tryLocal.fileName();
      
      VatsinatorApplication::log("File %s loaded from %s.",
                                 qPrintable(fname),
                                 qPrintable(tryLocal.fileName()));
      return;
    } else {
      VatsinatorApplication::log("File %s: checksum mismatch!", qPrintable(fname));
      VatsinatorApplication::log("  Expected: %s, got: %s",
                                 __manifest.hash[fname].md5.data(),
                                 md5Hash(tryLocal.fileName()).data());
    }
  }
  
  __files[_f] =
#ifndef Q_OS_DARWIN
    static_cast< QString >(VATSINATOR_PREFIX)
#else // on MacOS look for the file in the bundle
    QCoreApplication::applicationDirPath() % "/../Resources/"
#endif
    % fname;
}


