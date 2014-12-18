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
#include "events/decisionevent.h"
#include "network/plaintextdownloader.h"
#include "network/resourcemanager.h"
#include "network/statspurveyor.h"
#include "storage/cachefile.h"
#include "storage/languagemanager.h"
#include "storage/settingsmanager.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/userinterface.h"
#include "ui/vatsinatorstyle.h"
#include "ui/widgetsuserinterface.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/windows/settingswindow.h"
#include "ui/windows/vatsinatorwindow.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "storage/filemanager.h"
#include "config.h"

#include "vatsinatorapplication.h"

VatsinatorApplication::VatsinatorApplication(int& argc, char** argv) :
    QApplication(argc, argv),
    __userInterface(UserInterface::instantiate()),
    __fileManager(new FileManager()),
    __settingsManager(new SettingsManager()),
    __airlineDatabase(new AirlineDatabase()),
    __airportDatabaase(new AirportDatabase()),
    __firDatabase(new FirDatabase()),
    __worldMap(new WorldMap()),
    __vatsimData(new VatsimDataHandler()),
    __languageManager(new LanguageManager()),
    __resourceManager(new ResourceManager()),
    __statsPurveyor(new StatsPurveyor()) {
 
  /* Set up translations */
  QString locale = SettingsManager::earlyGetLocale();
  
  QTranslator* tr_qt = new QTranslator(this);
  tr_qt->load(QString("qt_") % locale, FileManager::staticPath(FileManager::Translations));
  installTranslator(tr_qt);
  
  QTranslator* tr = new QTranslator(this);
  tr->load(QString("vatsinator-") % locale, FileManager::staticPath(FileManager::Translations));
  installTranslator(tr);
  
  setStyle(new VatsinatorStyle());
  
  connect(this, &VatsinatorApplication::initializing, &VatsinatorApplication::__initialize);
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

bool
VatsinatorApplication::event(QEvent* event) {
  if (event->type() == Event::Decision) {
    userDecisionEvent(static_cast<DecisionEvent*>(event));
    return true;
  } else {
    return QApplication::event(event);
  }
}

void
VatsinatorApplication::restart() {
  /* http://stackoverflow.com/questions/5129788/how-to-restart-my-own-qt-application */
  qApp->quit();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}


void
VatsinatorApplication::userDecisionEvent(DecisionEvent* event) {
  if (event->context() == QStringLiteral("statistics")) {
    statsPurveyor()->setUserDecision(
      event->decision() == DecisionEvent::Accepted ? StatsPurveyor::Accepted : StatsPurveyor::Declined
    );
  }
}
void
VatsinatorApplication::__initialize() {
  qDebug("VatsinatorApplication: initializing");
  
  /* Read world map before UI */
  __worldMap->initialize();
  
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
  __airlineDatabase->initialize();
  __airportDatabaase->initialize();
  __firDatabase->initialize();
  
  /* Read data files only after databases are ready */
  __vatsimData->initialize();
  
  /* Initialize statistics */
  QSettings s;
  if (!s.contains("Decided/stats")) { // no decision made yet
    __userInterface->showStatsDialog();
  }
}
