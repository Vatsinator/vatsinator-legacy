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
    Outdated,
    Updating,
    Unknown
  };
  Q_ENUMS(ResourceManager::VersionStatus);
  
signals:
  
  /**
   * Emitted when it comes out that running instance of Vatsinator
   * is outdated.
   */
  void outdated();
  
  /**
   * Emitted after Vatsinator version has been checked.
   */
  void vatsinatorVersionChecked(ResourceManager::VersionStatus);
  
  /**
   * Emitted after database status has changed (i.e. Updated->Outdated->Updating->Updated).
   */
  void databaseStatusChanged(ResourceManager::VersionStatus);
  
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
  
  /**
   * Database sync will be checked only after the Vatsinator version
   * is checked and if the current version is up-to-date.
   */
  void __checkDatabase(ResourceManager::VersionStatus);
  
  /**
   * When Manifest file is fetched, we have to check it and move it
   * to the appropriate place on the local disk.
   */
  void __handleManifest(QString);
  
  /**
   * If an error occurs during downloading the manifest, this slot
   * will be called.
   */
  void __manifestError();
  
private:
  
  /**
   * Returns true if version1 is equal or higher that version2.
   * 
   * @param version1
   * @param version2
   * @return True if version1 >= version2.
   */
  bool __versionActual(const QString&, const QString&);
  
  /**
   * Fetches the manifest file.
   */
  void __fetchManifest();

};

#endif // RESOURCEMANAGER_H
