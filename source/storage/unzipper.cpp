/*
    unzipper.cpp
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
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "vatsinatorapplication.h"

#include "unzipper.h"

Unzipper::Unzipper(QString _fn, QObject* _parent) :
    QObject(_parent),
    __fileName(std::move(_fn)) {}

Unzipper::Unzipper(QObject* _parent) :
#ifdef GCC_VERSION_47
    Unzipper("", _parent) {} // yeah, I'm freak
#else
    QObject(_parent),
    __fileName("") {}
#endif

void
Unzipper::setFileName(const QString& _fn) {
    __fileName = _fn;
}

void
Unzipper::unzip() {
  if (__fileName.isEmpty()) {
    qCritical("Unzipper: error: file name not specified!");
    emit error("File name not specified!");
    return;
  }
  
  __fileList.clear();
  
  qDebug("Unzipper: unpacking archive %s", qPrintable(__fileName));
  QuaZip zip(__fileName);
  if (!zip.open(QuaZip::mdUnzip)) {
    qCritical("Unzipper: could not open zip file %s", qPrintable(__fileName));
    emit error(QString("Could not open zip file: %1").arg(__fileName));
    return;
  }
  
  int count = zip.getEntriesCount();
  int current = 1;
  
  /* Create directory that we will exclude files to */
  QDir tempDir(QDir::tempPath());
  QString targetSubDir("vatsinatordownload_" % QFileInfo(__fileName).fileName());
  if (!tempDir.mkdir(targetSubDir)) {
    qCritical("Unzipper: could not create temporary directory %s/%s",
              qPrintable(QDir::tempPath()), qPrintable(targetSubDir));
    emit error(QString("Could not create temporary directory: %1/%2")
        .arg(QDir::tempPath(), targetSubDir));
    return;
  }
  
  tempDir.cd(targetSubDir);
  __targetDir = tempDir.path() % "/";
  
  QuaZipFile file(&zip);
  for (bool f = zip.goToFirstFile(); f; f = zip.goToNextFile()) {
    QString fName = file.getActualFileName();
    if (fName.endsWith('/')) { // a directory
      if (!tempDir.mkpath(fName)) {
        qCritical("Unzipper: could not create directory %s/%s",
                  qPrintable(tempDir.dirName()), qPrintable(fName));
        emit error(QString("Could not create directory: %1/%2").arg(tempDir.dirName(), fName));
        return;
      }
    } else { // regular file
      qDebug("Unzipper: extracting %s...", qPrintable(fName));
      emit progress(count, current, fName);
      
      if (!file.open(QIODevice::ReadOnly)) {
        qCritical("Unzipper: could not access file %s", qPrintable(__fileName));
        emit error(QString("Could not access file %1").arg(__fileName));
        return;
      }
      
      QFile target(tempDir.path() % "/" % file.getActualFileName());
      if (!target.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qCritical("Unzipper: could not access file %s", qPrintable(target.fileName()));
        emit error(QString("Could not access file %1").arg(target.fileName()));
      }
      target.write(file.readAll());
      target.close();
      
      file.close();
      __fileList << fName;
    }
    
    ++current;
  }
  
  zip.close();
  
  emit unzipped();
}
