/*
    updatechecker.h
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


#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>

#include "singleton.h"

class PlainTextDownloader;

class UpdateChecker :
    public QObject,
    public Singleton< UpdateChecker > {
  
  /*
   * This class is used to check the current application version, the
   * newest stable release available and show appropriate notification.
   */
  
  Q_OBJECT
  
public:
  
  explicit UpdateChecker();
  virtual ~UpdateChecker();
  
  /**
   * Starts downloading data.
   */
  void init();
  
private:
  
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
  
  /* Running application version */
  Version __currentVersion;
  
  /* Version that is available to download */
  Version __newVersion;
  
  PlainTextDownloader* __httpHandler;
  
private slots:
  void __parseVersion(const QString&);
  
signals:
  /**
   * @param outdated If true, Vatsinator is outdated.
   */
  void versionChecked(bool);
  
};

#endif // UPDATECHECKER_H
