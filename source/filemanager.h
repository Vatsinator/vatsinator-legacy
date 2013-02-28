/*
    filemanager.h
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


#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QMutex>
#include <QMap>
#include <QDateTime>
#include <QIODevice>

#include "singleton.h"

class FileDownloader;

class FileManager : public Singleton< FileManager > {
  
  /*
   * This class stores vatsinator files' locations.
   */
  
public:
  enum File {
    AIRPORT_DB,
    FIR_DB,
    WORLD_DB,
    ALIAS,
    COUNTRY,
    FIR,
    MODEL,
    UIR,
    
    __COUNT   // items count
  };
  
  
  
  FileManager();
  virtual ~FileManager();
  
  /**
   * Returns real file location.
   */
  static const QString& path(FileManager::File);
    
  /**
   * From given enum, generates appropriate file name.
   */
  static QString enum2Str(FileManager::File);
  
  /**
   * Generates md5 hash of given file.
   */
  static QByteArray md5Hash(const QString&);
  static QByteArray md5Hash(QIODevice&);
  
private:
  class FileHash {
    
    /*
     * This class is used to describe one file's hash.
     */
  public:
    FileHash() = default;
    FileHash(const QByteArray&);
    
    QByteArray md5;
  };
  
  /**
   * Reads the manifest file.
   */
  void __readManifest(const QString&);
  
  /**
   * Checks if the particular file exists in local data store,
   * checks manifest and puts correct path in the __files array.
   */
  void __findFile(FileManager::File);
  
  
  QString  __files[__COUNT];
  
  struct {
    QMap< QString, FileHash > hash;
    QDateTime                 timestamp;
  } __manifest;
  
  FileDownloader* __downloader;
  
};

#endif // FILEMANAGER_H
