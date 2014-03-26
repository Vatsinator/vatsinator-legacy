/*
    vatsinatorapplication.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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


#ifndef VATSINATORAPPLICATION_H
#define VATSINATORAPPLICATION_H

#include <QApplication>
#include <QFont>
#include <QMutex>
#include <QTimer>
#include <iostream>

#include "singleton.h"

class AirportDatabase;
class CacheFile;
class FileManager;
class FirDatabase;
class LanguageManager;
class ModuleManager;
class ResourceManager;
class SettingsManager;
class StatsPurveyor;
class UserInterface;
class VatsimDataHandler;
class WorldMap;

class VatsinatorApplication :
    public QApplication,
    public Singleton<VatsinatorApplication> {

  /*
   * This class handles the whole Vatsinator application and has
   * only one instance.
   */

  Q_OBJECT
  
signals:
  void uiCreated();
  void dataDownloading();
  void metarsRefreshRequested();

public:
  /**
   * Constructor gives argc & argv to the QApplication.
   */
  VatsinatorApplication(int&, char**);

  virtual ~VatsinatorApplication();
  
  /**
   * Tell user that something went wrong.
   * @param msg Message to be displayed.
   */
  static void alert(const QString&, bool = false);
  
  static const QFont& boldFont();

#ifdef NO_DEBUG

  inline static void
  log(const char*) {}

  template <typename T, typename... Args>
  inline static void
  log(const char*, T, Args...) {}

#else

  static void log(const char*);

  template <typename T, typename... Args>
  static void log(const char* _s, T _value, Args... _args) {
    QMutexLocker l(&__mutex);
    
    while (*_s) {
      if (*_s == '%' && *(++_s) != '%') {
        std::cout << _value;
        VatsinatorApplication::log(*_s ? ++_s : _s, _args...);
        return;
      }

      std::cout << *_s++;
    }

    std::cout << std::endl;
  }

#endif

  inline VatsimDataHandler &
  vatsimData() { return *__vatsimData; }

public slots:
  void refreshData();
  void restart();
  
private slots:
  void __loadNewSettings();
  void __autoUpdatesToggled(bool);
  
private:
  
  FileManager*         __fileManager;
  AirportDatabase*     __airportsData;
  FirDatabase*         __firsData;
  WorldMap*            __worldMap;
  VatsimDataHandler*   __vatsimData;
  LanguageManager*     __languageManager;
  SettingsManager*     __settingsManager;
  ModuleManager*       __moduleManager;
  ResourceManager*     __resourceManager;
  StatsPurveyor*       __statsPurveyor;
  UserInterface*       __userInterface;
  
  QTimer               __timer;
  static QMutex        __mutex; /* For stdout */

};

#endif // VATSINATORAPPLICATION_H
