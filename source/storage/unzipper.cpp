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
#include "defines.h"

Unzipper::Unzipper(QString _fn, QObject* _parent) :
    QObject(_parent),
    __myThread(nullptr),
    __fileName(std::move(_fn)) {
  if (!_parent) {
    __myThread = new QThread(this);
    
    connect(__myThread, SIGNAL(started()),
            this,       SLOT(__unzip()));
    connect(this, SIGNAL(unzipped(Unzipper::UnzipStatus)),
                  SLOT(__restoreThread(Unzipper::UnzipStatus)));
  }
}

Unzipper::Unzipper(QObject* _parent) :
#ifdef GCC_VERSION_47
    Unzipper("", _parent) {}
#else
    QObject(_parent),
    __myThread(nullptr),
    __fileName("") {
  if (!_parent) {
    __myThread = new QThread(this);
    
    connect(__myThread, SIGNAL(started()),
            this,       SLOT(__unzip()));
    connect(this, SIGNAL(unzipped(Unzipper::UnzipStatus)),
                  SLOT(__restoreThread(Unzipper::UnzipStatus)));
  }
}
#endif

Unzipper::~Unzipper() {
  if (__myThread) {
    __myThread->quit();
    __myThread->deleteLater();
  }
}

void
Unzipper::unzip() {
  if (__myThread) {
    moveToThread(__myThread);
    __myThread->start();
  } else {
    __unzip();
  }
}

void
Unzipper::setFileName(const QString& _fn) {
    __fileName = _fn;
}

void
Unzipper::__unzip() {
  if (__fileName.isEmpty()) {
    VatsinatorApplication::log("Unzipper: error: file name not given, can not start unpacking");
    return;
  }
  
  __fileList.clear();
  
  QuaZip zip(__fileName);
  if (!zip.open(QuaZip::mdUnzip)) {
    emit error(tr("Could not open zip file: %1 (check your directory permissions)")
        .arg(__fileName));
    return;
  }
  
  int count = zip.getEntriesCount();
  int current = 1;
  
  /* Create directory that we will exclude files to */
  QDir tempDir(QDir::tempPath());
  QString targetSubDir("vatsinatordownload-" % zip.getZipName());
  tempDir.mkdir(targetSubDir);
  tempDir.cd(targetSubDir);
  
  QuaZipFile file(&zip);
  for (bool f = zip.goToFirstFile(); f; f = zip.goToNextFile()) {
    QString fName = file.getFileName();
    emit progress(count, current, fName);
    
    if (!file.open(QIODevice::ReadOnly)) {
      emit error(tr("Could not access zip archive (%1) properly. Skipping...")
          .arg(__fileName));
      current += 1;
      continue;
    }
    
    QFile target(tempDir.path() % "/" % file.getActualFileName());
    target.open(QIODevice::WriteOnly | QIODevice::Truncate);
    target.write(file.readAll());
    target.close();
    
    file.close();
    
    __fileList << target.fileName();
    ++current;
  }
  
  zip.close();
  
  emit unzipped(UNZIPPER_OK);
}

void
Unzipper::__restoreThread(Unzipper::UnzipStatus) {
  moveToThread(qApp->thread());
}
