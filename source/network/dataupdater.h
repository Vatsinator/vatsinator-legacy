/*
 * dataupdater.h
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

#ifndef DATAUPDATER_H
#define DATAUPDATER_H

#include <QObject>

class Unzipper;

/**
 * The DataUpdater class is responsible for synchronizing the local data
 * with the VatsinatorDatabase server.
 * 
 * \todo Rename to DatabaseUpdater.
 */
class DataUpdater : public QObject {
  Q_OBJECT
  
signals:
  /**
   * Emitted after files are downloaded, unzipped, checked and moved
   * to the destination directory correctly.
   */
  void updated();
  
  /**
   * Emitted when something goes wrong.
   */
  void failed();
  
  /**
   * This signal is for internal usage - starts unzipping files.
   */
  void readyToUnzip();

public:
  /**
   * The default constructor passes _parent_ to QObject.
   */
  explicit DataUpdater(QObject* parent = nullptr);
  
  /**
   * The destructor.
   */
  virtual ~DataUpdater();
  
public slots:
  /**
   * Begins the update process.
   * This method is thread-safe.
   */
  void update();
  
private:
  /*
   * Checks if every file unzipped has the correct checksum.
   */
  bool __checksumsOk(const QString& fileName);
  
  /**
   * Moves files from the temporary to the destination directory.
   */
  bool __moveFiles();
  
  /**
   * Removes all the data that could stay in the temporary directory.
   */
  void __cleanup();
  
private slots:
  
  /**
   * Starts unzipping the package.
   */
  void __unzipPackage(QString fileName);
  
  /**
   * Handles fetch errors (package's or Manifest's).
   */
  void __fetchError(QString error);
  
  /**
   * Fetches the Manifest.
   */
  void __filesUnzipped();
  
  /**
   * Something went wrong with the zip package.
   */
  void __unzipError(QString error);
  
  /**
   * Manifest downloaded, check md5 sums of files in the package.
   */
  void __checkManifest(QString fileName);
  
private:
  Unzipper* __unzipper;

};

#endif // DATAUPDATER_H
