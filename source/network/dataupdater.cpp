/*
 * dataupdater.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtGui>
#include <QtNetwork>

#include "network/filedownloader.h"

#include "storage/filemanager.h"
#include "storage/unzipper.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "dataupdater.h"
#include "defines.h"

static const QString PackageUrl = QString(NetConfig::Vatsinator::repoUrl()) % QString("packages/latest.zip");

namespace {
  
  /**
   * Moves the file.
   */
  bool moveFile(const QString& _oldLocation, const QString& _newLocation) {
    QFile file(_oldLocation);
    if (!file.open(QIODevice::ReadWrite)) {
      VatsinatorApplication::log("DataUpdater: failed accessing file %s", qPrintable(_oldLocation));
      return false;
    }
    
    file.close();
    
    QFile oldFile(_newLocation);
    if (oldFile.exists())
      oldFile.remove();
    
    QFileInfo fileInfo = QFileInfo(_newLocation);
    if (!QDir(fileInfo.path()).exists())
      QDir().mkpath(fileInfo.path());
    
    bool result = file.rename(_newLocation);
    if (result)
      VatsinatorApplication::log("DataUpdater: moved file %s -> %s", qPrintable(_oldLocation), qPrintable(_newLocation));
    else
      VatsinatorApplication::log("DataUpdater: failed moving %s -> %s", qPrintable(_oldLocation), qPrintable(_newLocation));
    
    return result;
  }
  
  /**
   * Removes the dir recursively.
   * http://stackoverflow.com/questions/11050977/removing-a-non-empty-folder-in-qt
   */
  bool removeDir(const QString& _dirName) {
    bool result = true;
    QDir dir(_dirName);
    
    if (dir.exists()) {
      for (QFileInfo file: dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
        if (file.isDir()) {
          VatsinatorApplication::log("DataUpdater: removing dir %s", qPrintable(file.absoluteFilePath()));
          result = removeDir(file.absoluteFilePath());
        } else {
          VatsinatorApplication::log("DataUpdater: removing file %s", qPrintable(file.absoluteFilePath()));
          result = QFile::remove(file.absoluteFilePath());
        }
        
        if (!result)
          return result;
      }
      
      result = dir.rmdir(_dirName);
    }
    
    return result;
  }
  
}

DataUpdater::DataUpdater(QObject* _parent) :
    QObject(_parent),
    __unzipper(new Unzipper()) {
  
  /* Give the Unzipper its own thread */
  QThread* thread = new QThread();
  __unzipper->moveToThread(thread);
  connect(__unzipper,   SIGNAL(unzipped()),
          this,         SLOT(__checkUnzipped()));
  connect(__unzipper,   SIGNAL(error(QString)),
          this,         SLOT(__unzipError(QString)));
  connect(this,         SIGNAL(readyToUnzip()),
          __unzipper,   SLOT(unzip()));
  thread->start();
}

DataUpdater::~DataUpdater() {
  QThread* thread = __unzipper->thread();
  __unzipper->deleteLater();
  thread->quit();
  thread->wait();
  delete thread;
}

void
DataUpdater::update() {
  VatsinatorApplication::log("DataUpdater: updating...");
  
  FileDownloader* fd = new FileDownloader();
  
  connect(fd,   SIGNAL(finished(QString)),
          this, SLOT(__unzipPackage(QString)));
  connect(fd,   SIGNAL(finished(QString)),
          fd,   SLOT(deleteLater()));
  connect(fd,   SIGNAL(error(QString)),
          this, SLOT(__fetchError(QString)));
  connect(fd,   SIGNAL(error(QString)),
          fd,   SLOT(deleteLater()));
  
  fd->fetch(QUrl(PackageUrl));
}

bool
DataUpdater::__moveFiles() {
  for (const QString& file: __unzipper->fileList()) {
    if (!moveFile(__unzipper->targetDir() % file, FileManager::localDataPath() % file))
      return false;
  }
  
  return true;
}

void
DataUpdater::__cleanup() {
  removeDir(__unzipper->targetDir());
  QFile::remove(__unzipper->fileName());
}

void
DataUpdater::__unzipPackage(QString _fileName) {
  __unzipper->setFileName(_fileName);
  emit readyToUnzip();
}

void
DataUpdater::__fetchError(QString _error) {
  VatsinatorApplication::log("DataUpdater: fetch error: %s", qPrintable(_error));
  emit failed();
}

void
DataUpdater::__checkUnzipped() {
  VatsinatorApplication::log("DataUpdater: files unzipped.");
  if (__moveFiles()) {
    __cleanup();
    emit updated();
  } else {
    emit failed();
  }
}

void
DataUpdater::__unzipError(QString _errStr) {
  VatsinatorApplication::log("DataUpdater: unzip error: %s", qPrintable(_errStr));
  emit failed();
}