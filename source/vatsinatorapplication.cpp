/*
    vatsinatorapplication.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include <cstdlib>

#include <iostream>

#include <QtGui>

#include "cache/cachefile.h"

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "db/worldmap.h"

#include "modules/flighttracker.h"
#include "modules/modulemanager.h"

#include "network/httphandler.h"

#include "settings/languagemanager.h"
#include "settings/settingsmanager.h"

#include "ui/userinterface.h"
#include "ui/windows/settingswindow.h"

#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "vatsinatorapplication.h"
#include "defines.h"

VatsinatorApplication::VatsinatorApplication(int& _argc, char** _argv) :
    QApplication(_argc, _argv),
    __airportsData(new AirportDatabase),
    __firsData(new FirDatabase),
    __worldMap(new WorldMap),
    __vatsimData(new VatsimDataHandler),
    __languageManager(new LanguageManager),
    __settingsManager(new SettingsManager),
    __modulesManager(new ModuleManager),
    __userInterface(NULL) {
  
#ifndef NO_DEBUG
#if defined Q_OS_LINUX
#  pragma message("Platform: Linux")
# elif defined Q_OS_WIN32
#  pragma message("Platform: Win32")
# elif defined Q_OS_DARWIN
#  pragma message("Platform: Darwin")
# else
#  pragma message("Platform: undefined")
# endif
#endif

  
  __translator.load(QString(TRANSLATIONS_DIR "/vatsinator-") + __settingsManager->getLanguage());
  this->installTranslator(&__translator);
  
  __userInterface = new UserInterface;

  // destroy all children windows before the program exits
  connect(this,             SIGNAL(destroyed()),
          __userInterface,  SLOT(hideAllWindows()));
  
  if (__settingsManager->cacheEnabled()) {
    connect(this,             SIGNAL(glInitialized()),
            this,             SLOT(__loadCachedData()));
  }

  // SettingsManager instance is now created, let him get the pointer & connect his slots
  __settingsManager->init();
  __modulesManager->init();

  // connect data refresher with the timer
  connect(&__timer, SIGNAL(timeout()), this, SLOT(refreshData()));

  // read .dat file
  __vatsimData->init();

  // if fetch goes wrong, show the alert
  connect(__vatsimData, SIGNAL(dataCorrupted()), this, SLOT(__showDataAlert()));

  // show main window
  __userInterface->show();

  // create something that will handle our http requests
  __httpHandler = new HttpHandler(__userInterface->getProgressBar());
  connect(__httpHandler, SIGNAL(finished(const QString&)), this, SLOT(__dataUpdated(const QString&)));
  connect(__httpHandler, SIGNAL(fetchError()), this, SLOT(__showDataAlert()));

  // fetch the status file
  refreshData();

  // start the timer
  __timer.start(__settingsManager->getRefreshRate() * 60000);
}

VatsinatorApplication::~VatsinatorApplication() {
  delete __settingsManager;
  delete __languageManager;
  delete __httpHandler;
  delete __vatsimData;
  delete __airportsData;
  delete __firsData;
  delete __worldMap;
  delete __userInterface;
  delete __modulesManager;

#ifndef NO_DEBUG
  DumpUnfreed();
#endif
}

void
VatsinatorApplication::alert(const QString& _msg, bool _fatal) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.exec();
  
  if (_fatal)
    quit();
}

void
VatsinatorApplication::emitGLInitialized() {
  VatsinatorApplication::GetSingleton().__emitGLInitialized();
}

#ifndef NO_DEBUG

void
VatsinatorApplication::log(const char* _s) {
  while (*_s) {
    Q_ASSERT(!(*_s == '%' && *(++_s) != '%'));
    std::cout << *_s++;
  }

  std::cout << std::endl;
}

#endif

void
VatsinatorApplication::refreshData() {
  __userInterface->toggleStatusBar();
  __httpHandler->fetchData(__vatsimData->getDataUrl());
  __timer.start(__settingsManager->getRefreshRate() * 60000);
}

void
VatsinatorApplication::dispatchDataUpdate(const QString& _fileName) {
  CacheFile file(_fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    VatsinatorApplication::log("File %s is not readable.", _fileName.toStdString().c_str());
    return;
  }
  
  QString data;
  
  while (!file.atEnd())
    data.append(file.readLine());
  file.close();
  
  VatsinatorApplication::GetSingleton().getData().parseDataFile(data);
  UserInterface::GetSingleton().infoBarUpdate();
}

void
VatsinatorApplication::__emitGLInitialized() {
  emit glInitialized();
}

void
VatsinatorApplication::__showDataAlert() {
  __userInterface->toggleStatusBar();
  
  QMessageBox decision;
  decision.setText(tr("Vatsinator was unable to fetch Vatsim's data file."));
  decision.setInformativeText(tr("What do you want to do with that?"));
  QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
  decision.addButton(tr("Keep current data"), QMessageBox::RejectRole);
  decision.setIcon(QMessageBox::Warning);

  __timer.stop();

  decision.exec();

  if (decision.clickedButton() == againButton)
    refreshData();
}

void
VatsinatorApplication::__loadCachedData() {
  CacheFile cache(CACHE_FILE_NAME);
  if (!cache.exists())
    return;
  
  QString fileName(CACHE_FILE_NAME);
  
  QtConcurrent::run(dispatchDataUpdate, fileName);
}

void
VatsinatorApplication::__dataUpdated(const QString& _data) {
  __userInterface->toggleStatusBar();
  
  if (_data.isEmpty()) {
    QMessageBox decision;
    decision.setText(tr("Vatsinator was unable to fetch Vatsim's data file."));
    decision.setInformativeText(tr("What do you want to do with that?"));
    QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
    decision.addButton(tr("Keep current data"), QMessageBox::RejectRole);
    decision.setIcon(QMessageBox::Warning);

    __timer.stop();

    decision.exec();

    if (decision.clickedButton() == againButton) {
      refreshData();
      return;
    } else {
      __userInterface->statusBarUpdate(tr("Data outdated!"));
      return;
    }
  }

  if (__vatsimData->statusFileFetched()) {
    __firsData->clearAll();

    __vatsimData->parseDataFile(_data);
    __userInterface->infoBarUpdate();
    __userInterface->statusBarUpdate();

    // we cannot depend on signals & slots system here, as GLrepaint() would be called
    // earlier, causing segfault
    FlightTracker::GetSingleton().updateData();

    if (__settingsManager->refreshMetars())
      emit metarsRefreshRequested();

    emit dataUpdated();
    
    if (__settingsManager->cacheEnabled()) {
      CacheFile cache(CACHE_FILE_NAME);
      cache.open(QIODevice::WriteOnly | QIODevice::Truncate);
      cache.write(_data.toUtf8());
      cache.close();
    }
  } else {
    __vatsimData->parseStatusFile(_data);

    __userInterface->statusBarUpdate();
    refreshData();
  }
}
