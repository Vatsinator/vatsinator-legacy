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

/**
 * The Unzipper class is useful when it comes to unzip any package harmlessy.
 * It provides nice and easy to use interface and comes with error reporting.
 * It is good idea to move the Unzipper to another thread, at least between
 * executing unzip() method and capturing the unzipped() signal.
 */
class Unzipper : public QObject {
  Q_OBJECT

signals:
  /**
   * Emited after next file is already unzipped.
   * 
   * \param total Total amount of files in the package.
   * \param unzipped Files already unzipped.
   * \param nextFile Going-to-be-unzipped file name.
   */
  void progress(int total, int unzipped, QString nextFile);
  
  /**
   * Emited when Unzipper is done with its job.
   */
  void unzipped();
  
  /**
   * Emited then an error occurs.
   * \param error Error string.
   */
  void error(QString error);
  
public:
  /**
   * The cosntructor takes the absolute zip archive location.
   */
  explicit Unzipper(QString fileName, QObject* parent = nullptr);
  
  /**
   * The default ctor.
   */
  explicit Unzipper(QObject* parent = nullptr);
  
  /**
   * Sets the zip package location. This function has no efect when
   * called after unzip() call.
   */
  void setFileName(const QString& fileName);
  
  /**
   * Gets the zip archive location.
   */
  inline const QString& fileName() const { return __fileName; }
  
  /**
   * Gets list of files that were already unzipped.
   */
  inline const QStringList& fileList() const { return __fileList; }
  
  /**
   * Gets the directory location where the unzipped files go.
   */
  inline const QString& targetDir() const { return __targetDir; }
  
public slots:
  
  /**
   * Starts extracting files from the archive. After that, fileName() function
   * has no effect, until the next execution of this method. The finished
   * process is signalized by the unzipped() signal.
   */
  void unzip();
  
private:
  QStringList __fileList;
  
  /* zip file name */
  QString __fileName;
  
  /* where all the files are extracted to */
  QString __targetDir;

};

#endif // UNZIPPER_H
