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

class DataUpdater : public QObject {
  
  Q_OBJECT
  
signals:
  void updated();
  void failed();
  void readyToUnzip();

public:
  explicit DataUpdater(QObject* = 0);
  virtual ~DataUpdater();
  
public slots:
  void update();
  
private:
  bool __checksumsOk(const QString&);
  bool __moveFiles();
  void __cleanup();
  
private slots:
  void __unzipPackage(QString);
  void __fetchError(QString);
  void __filesUnzipped();
  void __unzipError(QString);
  void __checkManifest(QString);
  
private:
  Unzipper* __unzipper;
  

};

#endif // DATAUPDATER_H
