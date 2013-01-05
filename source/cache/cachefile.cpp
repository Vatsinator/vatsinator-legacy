/*
    cachefile.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include "cachefile.h"
#include "defines.h"

#ifdef Q_OS_WIN32
static const QString CACHE_FILE_LOCATON(QDir::fromNativeSeparators(qgetenv("LOCALAPPDATA")));
#elif defined Q_OS_LINUX
static const QString CACHE_FILE_LOCATON(QDir::homePath() % "/.cache");
#elif defined Q_OS_DARWIN
static const QString CACHE_FILE_LOCATON(QDir::homePath() % "/Library/Caches");
#endif

static const QString CACHE_DIRECTORY(QDir::toNativeSeparators(CACHE_FILE_LOCATON % "/Vatsinator"));


CacheFile::CacheFile(const QString& _fileName) :
    QFile(CACHE_DIRECTORY % "/" % _fileName) {
  VatsinatorApplication::log("Cache file location: %s", fileName().toStdString().c_str());
}

bool
CacheFile::exists() const {
  if (!QDir(CACHE_DIRECTORY).exists()) {
    QDir().mkdir(CACHE_DIRECTORY);
    return false;
  }
  
  return QFile::exists();
}

bool
CacheFile::open(OpenMode _mode) {
  if (!QDir(CACHE_DIRECTORY).exists())
    QDir().mkdir(CACHE_DIRECTORY);
  
  bool wasOpened = QFile::open(_mode);
  if (!wasOpened)
    VatsinatorApplication::log("Cache file %s failed to open.", fileName().toStdString().c_str());
  
  return wasOpened;
}

