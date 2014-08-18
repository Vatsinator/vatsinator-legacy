/*
    vatsinatorapplication.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "db/airlinedatabase.h"
#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "db/worldmap.h"
#include "modules/modulemanager.h"
#include "network/plaintextdownloader.h"
#include "network/resourcemanager.h"
#include "network/statspurveyor.h"
#include "storage/cachefile.h"
#include "storage/languagemanager.h"
#include "storage/pluginmanager.h"
#include "storage/settingsmanager.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/userinterface.h"
#include "ui/vatsinatorstyle.h"
#include "ui/widgetsuserinterface.h"
#include "ui/windows/settingswindow.h"
#include "ui/windows/vatsinatorwindow.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "storage/filemanager.h"
#include "config.h"

#include "vatsinatorapplication.h"

VatsinatorApplication::VatsinatorApplication(int& _argc, char** _argv) :
    QApplication(_argc, _argv),
    __userInterface(new VATSINATOR_UI_IMPLEMENTATION()),
    __fileManager(new FileManager()),
    __settingsManager(new SettingsManager()),
    __pluginManager(new PluginManager(this)),
    __airlineDatabase(new AirlineDatabase()),
    __airportDatabaase(new AirportDatabase()),
    __firDatabase(new FirDatabase()),
    __worldMap(new WorldMap()),
    __vatsimData(new VatsimDataHandler()),
    __languageManager(new LanguageManager()),
    __moduleManager(new ModuleManager()),
    __resourceManager(new ResourceManager()),
    __statsPurveyor(new StatsPurveyor()) {
 
  /* Set up translations */
  QString locale = SettingsManager::earlyGetLocale();
  
  QTranslator* tr_qt = new QTranslator();
  tr_qt->load(QString("qt_") % locale, FileManager::staticPath(FileManager::Translations));
  installTranslator(tr_qt);
  connect(qApp,         SIGNAL(aboutToQuit()),
          tr_qt,        SLOT(deleteLater()));
  
  QTranslator* tr = new QTranslator();
  tr->load(QString("vatsinator-") % locale, FileManager::staticPath(FileManager::Translations));
  installTranslator(tr);
  connect(qApp,         SIGNAL(aboutToQuit()),
          tr,           SLOT(deleteLater()));
  
  setStyle(new VatsinatorStyle());
  
  connect(this, SIGNAL(initializing()), SLOT(__initialize()));
  emit initializing();
}

VatsinatorApplication::~VatsinatorApplication() {
  
  QThread* rmThread = __resourceManager->thread();
  __resourceManager->deleteLater();
  rmThread->quit();

  QThread* spThread = __statsPurveyor->thread();
  __statsPurveyor->deleteLater();
  spThread->quit();
  
  delete __settingsManager;
  delete __moduleManager;
  delete __languageManager;
  delete __vatsimData;
  delete __airportDatabaase;
  delete __firDatabase;
  delete __worldMap;
  delete __userInterface;
  delete __airlineDatabase;
  delete __fileManager;
  
  rmThread->wait();
  spThread->wait();
}

UserInterface*
VatsinatorApplication::userInterface() {
  Q_ASSERT(__userInterface);
  return __userInterface;
}

void
VatsinatorApplication::terminate() {
  std::terminate();
}

#ifndef QT_NO_DEBUG

void
VatsinatorApplication::log(const char* _s) {
  QMutexLocker l(&__mutex);
  
  while (*_s) {
    Q_ASSERT(!(*_s == '%' && *(++_s) != '%'));
    std::cout << *_s++;
  }

  std::cout << std::endl;
}

#endif

void
VatsinatorApplication::restart() {
  /* http://stackoverflow.com/questions/5129788/how-to-restart-my-own-qt-application */
  qApp->quit();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void
VatsinatorApplication::__initialize() {
  VatsinatorApplication::log("VatsinatorApplication: initializing");
  
  /* Read world map before UI */
  __worldMap->init();
  
  /* Load plugins */
  __pluginManager->loadPlugins();
  
  /* Create windows */
  __userInterface->initialize();
  
  /* Thread for ResourceManager */
  QThread* rmThread = new QThread(this);
  __resourceManager->moveToThread(rmThread);
  rmThread->start();
  
  /* Thread for StatsPurveyor */
  QThread* spThread = new QThread(this);
  __statsPurveyor->moveToThread(spThread);
  spThread->start();
 
  /* Initialize everything else */
  __airlineDatabase->init();
  __airportDatabaase->init();
  __firDatabase->init();
  
  /* Read data files only after databases are ready */
  __vatsimData->init();
}

QMutex VatsinatorApplication::__mutex(QMutex::Recursive);
