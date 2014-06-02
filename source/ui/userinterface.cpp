/*
    userinterface.cpp
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

#ifndef NO_DEBUG
# include "debugging/debugwindow.h"
#endif

#include "network/resourcemanager.h"
#include "modules/homelocation.h"
#include "storage/settingsmanager.h"
#include "ui/dialogs/apprestartdialog.h"
#include "ui/dialogs/datafetcherrordialog.h"
#include "ui/dialogs/newversiondialog.h"
#include "ui/dialogs/statusfetcherrordialog.h"
#include "ui/dialogs/vatsimmessagedialog.h"
#include "ui/windows/aboutwindow.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/atclistwindow.h"
#include "ui/windows/databasewindow.h"
#include "ui/windows/firdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "ui/windows/flightlistwindow.h"
#include "ui/windows/metarswindow.h"
#include "ui/windows/settingswindow.h"
#include "ui/windows/vatsinatorwindow.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/client.h"
#include "vatsinatorapplication.h"

#include "userinterface.h"
#include "defines.h"

UserInterface::UserInterface() :
#ifndef NO_DEBUG
    __debugWindow(nullptr),
#endif
    __aboutWindow(nullptr),
    __metarsWindow(nullptr),
    __databaseWindow(nullptr),
    __firDetailsWindow(nullptr),
    __atcDetailsWindow(nullptr),
    __atcListWindow(nullptr),
    __flightDetailsWindow(nullptr),
    __flightsListWindow(nullptr),
    __settingsWindow(nullptr),
    __vatsinatorWindow(nullptr) {}

UserInterface::~UserInterface() {
  delete __vatsinatorWindow;
  delete __aboutWindow;
  delete __databaseWindow;
  delete __firDetailsWindow;
  delete __flightDetailsWindow;
  delete __flightsListWindow;
  delete __atcDetailsWindow;
  delete __atcListWindow;
  delete __settingsWindow;
  delete __metarsWindow;

#ifndef NO_DEBUG
  delete __debugWindow;
#endif
}

void
UserInterface::init() {
#ifndef NO_DEBUG
  __debugWindow = new DebugWindow();
#endif
  
  __aboutWindow = new AboutWindow();
  __metarsWindow = new MetarsWindow();
  __databaseWindow = new DatabaseWindow();
  __firDetailsWindow = new FirDetailsWindow();
  __atcDetailsWindow = new AtcDetailsWindow();
  __atcListWindow = new AtcListWindow();
  __flightDetailsWindow = new FlightDetailsWindow();
  __flightsListWindow = new FlightListWindow();
  __settingsWindow = new SettingsWindow();
  __vatsinatorWindow = new VatsinatorWindow();
  
  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(vatsimDataError()),
          this,                                     SLOT(__dataError()));
  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(vatsimStatusError()),
          this,                                     SLOT(__statusError()));
  connect(ResourceManager::getSingletonPtr(),       SIGNAL(outdated()),
          this,                                     SLOT(__showNewVersionDialog()));
}

void
UserInterface::fatal(const QString& _msg) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.setIcon(QMessageBox::Critical);
  
  VatsinatorApplication::log(qPrintable(_msg));
  
  msgBox.exec();
  
  VatsinatorApplication::terminate();
}

void
UserInterface::warning(const QString& _msg) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.setIcon(QMessageBox::Warning);
  
  VatsinatorApplication::log(qPrintable(_msg));
  
  msgBox.exec();
}

void
UserInterface::showAppRestartDialog() {
  AppRestartDialog* dialog = new AppRestartDialog();
  
  connect(dialog,                                       SIGNAL(accepted()),
          VatsinatorApplication::getSingletonPtr(),     SLOT(restart()));
  connect(dialog,                                       SIGNAL(finished(int)),
          dialog,                                       SLOT(deleteLater()));
  
  dialog->show();
  dialog->raise();
  dialog->activateWindow();  
}

void
UserInterface::showVatsimMessage(const QString& _msg) {
  QSettings s;
  if (s.value("VatsimMessages/" % _msg, false).toBool())
    return;
  
  VatsimMessageDialog* dialog = new VatsimMessageDialog(_msg);
  
  connect(dialog,       SIGNAL(finished(int)),
          dialog,       SLOT(deleteLater()));
  
  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void
UserInterface::showDetailsWindow(const Airport* _ap) {
  (new AirportDetailsWindow(_ap))->show();
}

void
UserInterface::__statusError() {
  StatusFetchErrorDialog dialog;
  dialog.exec();
}

void
UserInterface::__dataError() {
  DataFetchErrorDialog dialog;
  dialog.exec();
  
  if (dialog.clickedButton() == dialog.again()) {
    // TODO
//     VatsinatorApplication::getSingleton().refreshData();
  }
}

void
UserInterface::__showNewVersionDialog() {
  NewVersionDialog dialog(__vatsinatorWindow);
  dialog.exec();
}
