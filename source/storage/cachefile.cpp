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

#include <QtCore>

#include "vatsinatorapplication.h"

#include "cachefile.h"

static const QString CacheDirectory =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDir::cleanPath(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
      % QDir::separator()
      % "Vatsinator"
    );
#else
    QDir::cleanPath(
      QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
      % QDir::separator()
      % "Vatsinator"
    );
#endif

CacheFile::CacheFile(const QString& _fileName) :
    QFile(CacheDirectory % QDir::separator() % _fileName) {
  qDebug("Cache file location: %s", qPrintable(fileName()));
}

bool
CacheFile::exists() const {
  if (!QDir(CacheDirectory).exists()) {
    qDebug("CacheFile: creating directory %s...", qPrintable(CacheDirectory));
    QDir().mkpath(CacheDirectory);
    return false;
  }
  
  return QFile::exists();
}

bool
CacheFile::open(OpenMode _mode) {
  if (!QDir(CacheDirectory).exists())
    QDir().mkdir(CacheDirectory);
  
  bool wasOpened = QFile::open(_mode);
  if (!wasOpened)
    qWarning("CacheFile: %s failed to open.", qPrintable(fileName()));
  
  return wasOpened;
}
