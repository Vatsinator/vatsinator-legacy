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


FileManager::FileManager() {
  VatsinatorApplication::log("Local data location: %s", qPrintable(DATA_LOCATION));
}

const QString &
FileManager::path(FileManager::File _f) {
  if (FileManager::getSingleton().__files[_f].isEmpty())
    FileManager::getSingleton().__findFile(_f);
  
  return FileManager::getSingleton().__files[_f];
}

void
FileManager::__findFile(FileManager::File _f) {
  QString fname;
  
  switch (_f) {
    case AIRPORT_DB:
      fname = "WorldAirports.db";
      break;
    
    case FIR_DB:
      fname = "WorldFirs.db";
      break;
      
    case WORLD_DB:
      fname = "WorldMap.db";
      break;
      
    case ALIAS:
      fname = "data/alias";
      break;
      
    case COUNTRY:
      fname = "data/country";
      break;
    
    case FIR:
      fname = "data/fir";
      break;
      
    case MODEL:
      fname = "data/model";
      break;
      
    case UIR:
      fname = "data/uir";
      break;
      
    case MANIFEST:
      fname = "Manifest";
      break;
      
    default:
      Q_ASSERT(false);
      break;
  }
  
  QFile tryLocal(DATA_LOCATION % fname);
  if (tryLocal.exists()) {
    __files[_f] = tryLocal.fileName();
    
    VatsinatorApplication::log("File %s loaded from %s.", qPrintable(fname), qPrintable(tryLocal.fileName()));
  } else {
    __files[_f] =
#ifndef Q_OS_DARWIN
      static_cast< QString >(VATSINATOR_PREFIX)
#else // on MacOS look for the file in the bundle
      QCoreApplication::applicationDirPath() % "/../Resources/"
#endif
      % fname;
  }
}
