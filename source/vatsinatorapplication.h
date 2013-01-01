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
#include <QTranslator>
#include <QTimer>
#include <iostream>

#include "singleton.h"

class AirportDatabase;
class CacheFile;
class FirDatabase;
class UserInterface;
class VatsimDataHandler;
class LanguageManager;
class SettingsManager;
class WorldMap;
class HttpHandler;
class ModuleManager;

class VatsinatorApplication :
    public QApplication,
    public Singleton< VatsinatorApplication > {

  /*
   * This class handles the whole Vatsinator application and has
   * only one instance.
   */

  Q_OBJECT

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

  static void emitGLInitialized();
  
  /**
   * Use this function in QtConcurrent::run to read data
   * stored in cache file in separated thread.
   * @param fileName Name of the file in cache.
   */
  static void dispatchDataUpdate(const QString&);

#ifdef NO_DEBUG

  inline static void
  log(const char*) {}

  template< typename T, typename... Args >
  inline static void
  log(const char*, T, Args...) {}

#else

  static void log(const char*);

  template< typename T, typename... Args >
  static void log(const char* _s, T _value, Args... _args) {
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
  getData() { return *__vatsimData; }

public slots:
  void refreshData();

private:
  void __emitGLInitialized();

  HttpHandler*       __httpHandler;
  AirportDatabase*   __airportsData;
  FirDatabase*       __firsData;
  WorldMap*          __worldMap;
  VatsimDataHandler* __vatsimData;
  LanguageManager*   __languageManager;
  SettingsManager*   __settingsManager;
  ModuleManager*     __modulesManager;
  UserInterface*     __userInterface;
  QTimer             __timer;
  QTranslator        __translator;

private slots:
  void __loadCachedData();
  void __dataUpdated(const QString&);
  void __showDataAlert();
  void __loadNewSettings();
  void __autoUpdatesToggled(bool);
  
signals:
  void glInitialized();
  void dataDownloading();
  void dataUpdated();
  void metarsRefreshRequested();

};

#endif // VATSINATORAPPLICATION_H
