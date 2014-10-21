/*
 * resourcemanager.h
 * Copyright (C) 2013-2014  Michał Garapich <michal@garapich.pl>
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

/**
 * The ResourceManager class is responsible for keeping local files in sync
 * with the online resources. It checks whether Vatsinator is up-to-date
 * or not, downloads updated data files, etc.
 */
class ResourceManager : public QObject {
  Q_OBJECT
  
public:
  /**
   * The VersionStatus enum is used in version control.
   * Vatsinator version indicator uses only the two values: Updated
   * and Outdated, whilst the database sync indicator uses all of
   * them.
   */
  enum VersionStatus {
    Updated,            /**< Up-to-date */
    Outdated,           /**< Needs update */
    Updating,           /**< Is being updated right now */
    CannotUpdate        /**< Cannot update because of a reason */
  };
  Q_ENUMS(ResourceManager::VersionStatus)
  
signals:
  /**
   * Emitted when it comes out that running instance of Vatsinator
   * is outdated.
   */
  void outdated();
  
  /**
   * Emitted after Vatsinator version has been checked.
   */
  void vatsinatorVersionChecked(ResourceManager::VersionStatus status);
  
  /**
   * Emitted after database status has changed (i.e. Updated->Outdated->Updating->Updated).
   */
  void databaseStatusChanged(ResourceManager::VersionStatus status);
  
public:
  /**
   * The default constructor passes _parent_ to QObject.
   */
  explicit ResourceManager(QObject* parent = nullptr);
  
  virtual ~ResourceManager();
  
  /**
   * Returns date of the last update.
   */
  inline const QDate& lastUpdateDate() const {
    return __lastUpdateDate;
  }
  
public slots:
  /**
   * If possible, synchronizes the database.
   */
  void requestDatabaseSync();

private:
  /**
   * Returns true if _version1_ is equal or higher that _version2_.
   */
  bool __versionActual(const QString& version1, const QString& version2);

private slots:
  
  /**
   * Starts fetching the new version from Vatsinator repository.
   */
  void __fetchVersion();
  
  /**
   * After fetching the new version string, parses it and displays
   * appropriate notification.
   */
  void __parseVersion(QString version);
  
  /**
   * Database sync will be checked only after the Vatsinator version
   * is checked and if the current version is up-to-date.
   */
  void __checkDatabase(ResourceManager::VersionStatus status);
  
    /**
   * Starts the data updater.
   */
  void __syncDatabase();
  
  void __databaseUpdated();
  void __databaseFailed();
  
private:
  
  /* The date of the last database update */
  QDate __lastUpdateDate;

};

#endif // RESOURCEMANAGER_H
