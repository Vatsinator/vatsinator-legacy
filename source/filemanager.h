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

#include "singleton.h"

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
    MANIFEST,
    
    __COUNT   // items count
  };
  
  
  
  FileManager();
  
  /**
   * Returns real file location.
   */
  static const QString& path(FileManager::File);
  
private:
  void __findFile(FileManager::File);
  
  QString __files[__COUNT];
  
};

#endif // FILEMANAGER_H
