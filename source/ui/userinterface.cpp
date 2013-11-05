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

#include "ui/windows/aboutwindow.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/atclistwindow.h"
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
    __debugWindow(new DebugWindow()),
#endif
    __aboutWindow(new AboutWindow()),
    __metarsWindow(new MetarsWindow()),
    __airportDetailsWindow(new AirportDetailsWindow()),
    __firDetailsWindow(new FirDetailsWindow()),
    __atcDetailsWindow(new AtcDetailsWindow()),
    __atcListWindow(new AtcListWindow()),
    __flightDetailsWindow(new FlightDetailsWindow()),
    __flightsListWindow(new FlightListWindow()),
    __settingsWindow(new SettingsWindow()),
    __vatsinatorWindow(new VatsinatorWindow()) {


  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(dataCorrupted()),
          this,                                     SLOT(__fetchError()));
  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(vatsimStatusError()),
          this,                                     SLOT(__statusFileError()));
  connect(ResourceManager::getSingletonPtr(),       SIGNAL(outdated()),
          this,                                     SLOT(__showNewVersionDialog()));
}

UserInterface::~UserInterface() {

  delete __vatsinatorWindow;
  delete __aboutWindow;
  delete __airportDetailsWindow;
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
UserInterface::__statusFileError() {
  StatusFetchErrorDialog dialog;
  dialog.exec();
}

void
UserInterface::__fetchError() {
  DataFetchErrorDialog dialog;
  dialog.exec();
  
  if (dialog.clickedButton() == dialog.again()) {
    VatsinatorApplication::getSingleton().refreshData();
  }
}

void
UserInterface::__showNewVersionDialog() {
  NewVersionDialog dialog(__vatsinatorWindow);
  dialog.exec();
}
