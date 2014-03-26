/*
    vatsinatorapplication.cpp
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

#include <QtGui>

#include "storage/cachefile.h"

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "db/worldmap.h"

#include "modules/modulemanager.h"

#include "network/plaintextdownloader.h"
#include "network/resourcemanager.h"
#include "network/statspurveyor.h"

#include "storage/languagemanager.h"
#include "storage/settingsmanager.h"

#include "ui/pages/miscellaneouspage.h"

#include "ui/userinterface.h"
#include "ui/windows/settingswindow.h"
#include "ui/windows/vatsinatorwindow.h"

#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "storage/filemanager.h"

#include "vatsinatorapplication.h"
#include "defines.h"

VatsinatorApplication::VatsinatorApplication(int& _argc, char** _argv) :
    QApplication(_argc, _argv),
    __fileManager(new FileManager()),
    __airportsData(new AirportDatabase()),
    __firsData(new FirDatabase()),
    __worldMap(new WorldMap()),
    __vatsimData(new VatsimDataHandler()),
    __languageManager(new LanguageManager()),
    __settingsManager(new SettingsManager()),
    __moduleManager(new ModuleManager()),
    __resourceManager(new ResourceManager()),
    __statsPurveyor(new StatsPurveyor()),
    __userInterface(nullptr) {
 
  /* Set up translations */
  QTranslator* tr_qt = new QTranslator();
  tr_qt->load(QString("qt_") %
                SettingsManager::earlyGetLocale(),
              FileManager::staticPath(FileManager::Translations));
  installTranslator(tr_qt);
  connect(qApp,         SIGNAL(aboutToQuit()),
          tr_qt,        SLOT(deleteLater()));
  
  QTranslator* tr = new QTranslator();
  tr->load(QString("vatsinator-") %
             SettingsManager::earlyGetLocale(),
           FileManager::staticPath(FileManager::Translations));
  installTranslator(tr);
  connect(qApp,         SIGNAL(aboutToQuit()),
          tr,           SLOT(deleteLater()));
  
  
  /* Basic initializes */
  QtConcurrent::run(__vatsimData, &VatsimDataHandler::init);
  
  __userInterface = new UserInterface();
  __settingsManager->init();
//   __moduleManager->init();
  
  // connect EnableAutoUpdatesAction toggle
  connect(VatsinatorWindow::getSingletonPtr(),  SIGNAL(autoUpdatesEnabled(bool)),
          this,                                 SLOT(__autoUpdatesToggled(bool)));
  
  // handle settings changes
  connect(__settingsManager,                    SIGNAL(settingsChanged()),
          this,                                 SLOT(__loadNewSettings()));

  // connect data refresher with the timer
  connect(&__timer,                             SIGNAL(timeout()),
          this,                                 SLOT(refreshData()));
  
  // when status file is fetched, we may start fetching the data
  connect(__vatsimData,                         SIGNAL(vatsimStatusUpdated()),
          this,                                 SLOT(refreshData()));

  // show main window
  VatsinatorWindow::getSingleton().show();
  emit uiCreated();
  
  __timer.setInterval(SM::get("network.refresh_rate").toInt() * 60000);
  
  /* Thread for ResourceManager */
  QThread* rmThread = new QThread(this);
  __resourceManager->moveToThread(rmThread);
  rmThread->start();
  
  /* Thread for StatsPurveyor */
  QThread* spThread = new QThread(this);
  __statsPurveyor->moveToThread(spThread);
  spThread->start();
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
  delete __airportsData;
  delete __firsData;
  delete __worldMap;
  delete __userInterface;
  delete __fileManager;
  
  rmThread->wait();
  delete rmThread;
  
  spThread->wait();
  delete spThread;

#ifndef NO_DEBUG
  DumpUnfreed();
#endif
}

void
VatsinatorApplication::alert(const QString& _msg, bool _fatal) {
  if (getSingleton().__userInterface) {
    QMessageBox msgBox;
    msgBox.setText(_msg);
    msgBox.setIcon(_fatal ? QMessageBox::Critical : QMessageBox::Warning);
    msgBox.exec();
  }
  
  VatsinatorApplication::log(qPrintable(_msg));
  
  if (_fatal)
    QCoreApplication::exit(1);
}

const QFont &
VatsinatorApplication::boldFont() {
  static QFont font;
  font.setBold(true);
  return font;
}

#ifndef NO_DEBUG

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
VatsinatorApplication::refreshData() {
  emit dataDownloading();
  
  if (!VatsinatorWindow::getSingleton().autoUpdatesEnabled())
    __timer.stop();
  else
    __timer.start();
}

void
VatsinatorApplication::restart() {
  /* http://stackoverflow.com/questions/5129788/how-to-restart-my-own-qt-application */
  qApp->quit();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void
VatsinatorApplication::__loadNewSettings() {
  int rr = SM::get("network.refresh_rate").toInt();
  if (__timer.interval() / 60000 != rr) {
    __timer.setInterval(rr * 60000);
    
    if (VatsinatorWindow::getSingleton().autoUpdatesEnabled())
      refreshData();
  }
}

void
VatsinatorApplication::__autoUpdatesToggled(bool _state) {
  if (!_state) {
    __timer.stop();
  } else {
    if (!__timer.isActive())
      refreshData();
  }
}

QMutex VatsinatorApplication::__mutex(QMutex::Recursive);
