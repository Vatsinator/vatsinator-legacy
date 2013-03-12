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

class QThread;

class Unzipper : public QObject {
  
  /*
   * This class is used to unzip any .zip package in the separate thread. 
   */
  
  Q_OBJECT
  
public:
  
  enum UnzipStatus {
    UNZIPPER_OK,
    UNZIPPER_FAIL
  };
  
  explicit Unzipper(QString, QObject* = 0);
  explicit Unzipper(QObject* = 0);
  virtual ~Unzipper();
  
  void unzip();
  
  void setFileName(const QString&);
  
  inline const QString &
  getFileName() const { return __fileName; }
  
  inline const QStringList &
  getFileList() const { return __fileList; }
  
private:
  QThread*    __myThread;
  QStringList __fileList;
  
  /* zip file name */
  QString     __fileName;
  
private slots:
  void __unzip();
  void __restoreThread(Unzipper::UnzipStatus);
  
signals:
  /**
   * Emited after next file is already unzipped.
   * @param total Total amount of files in the package.
   * @param unzipped Files already unzipped.
   * @param nextFile Going-to-be-unzipped file name.
   */
  void progress(int, int, QString);
  
  /**
   * @param fileList List of files that were extraced from the package.
   * NOTE: Files are listed with their path as it is stored in zip.
   * Example: "foo.txt", "bar/bar.txt", "bar/baz/baz.txt"
   */
  void unzipped(Unzipper::UnzipStatus);
  
  /**
   * Emited then an error occurs.
   * @param erStr Error status.
   */
  void error(QString);
  
};

#endif // UNZIPPER_H
