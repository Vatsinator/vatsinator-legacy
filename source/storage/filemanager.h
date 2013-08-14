/*
    storage/filemanager.h
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

class FileManager : public Singleton<FileManager> {
  
  /*
   * This class stores vatsinator files' locations.
   */
  
public:
  
  FileManager();
  virtual ~FileManager() = default;
  
  /**
   * Stores given data in local cache file.
   * @param fileName File name. Only file name, not full path.
   * @param data Data to be saved.
   */
  static void cacheData(const QString&, const QString&);
  
  /**
   * Returns a real file location.
   * 
   * Vatsinator can store its data files in two locations: at the global scope
   * (i.e. /usr/share/vatsinator/ on Linux) and locally (after data update,
   * for example ~/.local/share/vatsinator/). This function makes it not only
   * transparent (every file can be stored in both locations, local files have
   * got higher priority, however) but it also makes it cross-platform (on
   * Windows, for example, globally-scoped files are stored in Program Files
   * directory).
   */
  static QString path(const QString&);
  
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
  
  struct {
    QMap<QString, FileHash> hash;
    QDateTime               timestamp;
  } __manifest;
  
};

#endif // FILEMANAGER_H
