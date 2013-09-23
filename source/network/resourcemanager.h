/*
 * resourcemanager.h
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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QDateTime>
#include <QObject>

#include "singleton.h"

class QThread;

class ResourceManager :
    public QObject,
    public Singleton<ResourceManager> {
  
  /**
   * This class is responsible for manipulating local resources.
   * It checks whether Vatsinator is up-to-date or not, downloads
   * updated data files, etc.
   */
  
  Q_OBJECT
  
public:
  
  enum VersionStatus {
    Updated,
    Outdated
  };
  Q_ENUMS(ResourceManager::VersionStatus);
  
signals:
  
  /**
   * Emitted when it comes out that running instance of Vatsinator
   * is outdated.
   */
  void outdated();
  
  /**
   * Emitted after version has been checked.
   */
  void versionChecked(ResourceManager::VersionStatus);
  
public:
  
  explicit ResourceManager(QObject* = 0);
  
  virtual ~ResourceManager();

private slots:
  
  /**
   * Starts fetching the new version from Vatsinator repository.
   */
  void __fetchVersion();
  
  /**
   * After fetching the new version string, parses it and displays
   * appropriate notification.
   */
  void __parseVersion(QString);
  
private:
  
  /* The thread that this ResourceManager will be running on */
  QThread* __myThread;
  
  class Version {
    
  public:
    
    /**
     * Parses version string.
     */
    Version(const QString&);
    
    /**
     * minor = -1, major = -1
     */
    Version();
    
    bool operator <(const Version&);
    
  private:
    
    /* {major}.{minor} */
    int __major;
    int __minor;
  };
  
  Version __currentVersion;
  

};

#endif // RESOURCEMANAGER_H
