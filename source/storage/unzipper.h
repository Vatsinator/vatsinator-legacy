/*
    unzipper.h
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


#ifndef UNZIPPER_H
#define UNZIPPER_H

#include <QObject>
#include <QStringList>

class Unzipper : public QObject {
  
  /*
   * This class is used to unzip any .zip package in the separate thread. 
   */
  
  Q_OBJECT
  
public:

signals:
  /**
   * Emited after next file is already unzipped.
   * @param total Total amount of files in the package.
   * @param unzipped Files already unzipped.
   * @param nextFile Going-to-be-unzipped file name.
   */
  void progress(int, int, QString);
  
  /**
   * Emited when Unzipper is done with its job.
   */
  void unzipped();
  
  /**
   * Emited then an error occurs.
   * @param erStr Error string.
   */
  void error(QString);
  
public:
  
  explicit Unzipper(QString, QObject* = 0);
  explicit Unzipper(QObject* = 0);
  
  void setFileName(const QString&);
  
  inline const QString &
  fileName() const { return __fileName; }
  
  inline const QStringList &
  fileList() const { return __fileList; }
  
  inline const QString &
  targetDir() const { return __targetDir; }
  
public slots:
  void unzip();
  
private:
  QStringList __fileList;
  
  /* zip file name */
  QString __fileName;
  
  /* where all the files are extracted to */
  QString __targetDir;

};

#endif // UNZIPPER_H
