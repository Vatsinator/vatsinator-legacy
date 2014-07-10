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
    VatsinatorApplication::log("Unzipper: error: file name not specified!");
    emit error("File name not specified!");
    return;
  }
  
  __fileList.clear();
  
  VatsinatorApplication::log("Unzipper: unpacking archive %1", qPrintable(__fileName));
  QuaZip zip(__fileName);
  if (!zip.open(QuaZip::mdUnzip)) {
    emit error(QString("Could not open zip file: %1").arg(__fileName));
    return;
  }
  
  int count = zip.getEntriesCount();
  int current = 1;
  
  /* Create directory that we will exclude files to */
  QDir tempDir(QDir::tempPath());
  QString targetSubDir("vatsinatordownload_" % QFileInfo(__fileName).fileName());
  if (!tempDir.mkdir(targetSubDir)) {
    emit error(QString("Could not create temporary directory: %1/%2").arg(QDir::tempPath(), targetSubDir));
    return;
  }
  
  tempDir.cd(targetSubDir);
  __targetDir = tempDir.path() % "/";
  
  QuaZipFile file(&zip);
  for (bool f = zip.goToFirstFile(); f; f = zip.goToNextFile()) {
    QString fName = file.getActualFileName();
    if (fName.endsWith('/')) { // a directory
      if (!tempDir.mkpath(fName)) {
        emit error(QString("Could not create directory: %1/%2").arg(tempDir.dirName(), fName));
        return;
      }
    } else { // regular file
      VatsinatorApplication::log("Unzipper: extracting %1...", qPrintable(fName));
      emit progress(count, current, fName);
      
      if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Could not access file %1").arg(__fileName));
        return;
      }
      
      QFile target(tempDir.path() % "/" % file.getActualFileName());
      if (!target.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
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
