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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "db/worldmap.h"

#include "modules/flighttracker.h"
#include "modules/modulemanager.h"

#include "network/httphandler.h"

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
    __settingsManager(new SettingsManager),
    __modulesManager(new ModuleManager),
    __userInterface(NULL) {
  
  log("vatsinator.dat location: " VATSINATOR_DAT);
  
#ifndef NO_DEBUG
# ifdef VATSINATOR_PLATFORM_LINUX
#  pragma message("Platform: Linux")
# elif defined VATSINATOR_PLATFORM_WIN32
#  pragma message("Platform: Win32")
# else
#  pragma message("Platform: undefined")
# endif
#endif
  
  QString lang = QLocale::system().name();
  
  __translator.load(QString(TRANSLATIONS_DIR "/vatsinator-") + lang);
  this->installTranslator(&__translator);
  
  __userInterface = new UserInterface;

  // destroy all children windows before the program exits
  connect(this,             SIGNAL(destroyed()),
          __userInterface,  SLOT(hideAllWindows()));

  // SettingsManager instance is now created, let him get the pointer & connect his slots
  __settingsManager->init();
  __modulesManager->init();

  // connect data refresher with the timer
  connect(&__timer, SIGNAL(timeout()), this, SLOT(refreshData()));

  // read database
  __airportsData->init();

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
VatsinatorApplication::quit() {
  exit(0);
}

void
VatsinatorApplication::alert(const QString& _msg) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.exec();
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
  __userInterface->statusBarUpdate(tr("Fetching data..."));
  __httpHandler->fetchData(__vatsimData->getDataUrl());
  __timer.start(__settingsManager->getRefreshRate() * 60000);
}

void
VatsinatorApplication::__emitGLInitialized() {
  emit glInitialized();
}

void
VatsinatorApplication::__showDataAlert() {
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
VatsinatorApplication::__dataUpdated(const QString& _data) {
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
    __userInterface->getClientsBox()->setText(
      tr("Clients") % static_cast< QString >(": ") %
      QString::number(__vatsimData->clientCount()) %
      static_cast< QString >(" (") %
      QString::number(__vatsimData->pilotCount()) %
      " " % tr("pilots") % static_cast< QString >(", ") %
      QString::number(__vatsimData->atcCount()) %
      " " % tr("ATCs") % static_cast< QString >(", ") %
      QString::number(__vatsimData->obsCount()) %
      " " % tr("observers") % static_cast< QString >(")")
    );

    __userInterface->statusBarUpdate();

    // we cannot depend on signals & slots system here, as GLrepaint() would be called
    // earlier, causing segfault
    FlightTracker::GetSingleton().updateData();

    if (__settingsManager->refreshMetars())
      emit metarsRefreshRequested();

    emit dataUpdated();
  } else {
    __vatsimData->parseStatusFile(_data);

    __userInterface->statusBarUpdate();
    refreshData();
  }
}

