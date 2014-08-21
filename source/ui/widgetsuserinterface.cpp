/*
 * widgetsuserinterface.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

#include "network/resourcemanager.h"
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
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsinatorapplication.h"

#include "widgetsuserinterface.h"

WidgetsUserInterface::WidgetsUserInterface(QObject* _parent):
    UserInterface(_parent) {}

WidgetsUserInterface::~WidgetsUserInterface() {
  delete __vatsinatorWindow;
  delete __aboutWindow;
  delete __databaseWindow;
  delete __flightsListWindow;
  delete __atcListWindow;
  delete __settingsWindow;
  delete __metarsWindow;
}

void
WidgetsUserInterface::initialize() {
  __aboutWindow = new AboutWindow();
  __metarsWindow = new MetarsWindow();
  __databaseWindow = new DatabaseWindow();
  __atcListWindow = new AtcListWindow();
  __flightsListWindow = new FlightListWindow();
  __settingsWindow = new SettingsWindow();
  __vatsinatorWindow = new VatsinatorWindow();
  
  connect(ResourceManager::getSingletonPtr(),       SIGNAL(outdated()),
          this,                                     SLOT(__showNewVersionDialog()));
  
  emit initialized();
  
  mainWindow()->show();
}

AboutWindow*
WidgetsUserInterface::aboutWindow() {
  Q_ASSERT(__aboutWindow);
  return __aboutWindow;
}

AtcListWindow*
WidgetsUserInterface::atcListWindow() {
  Q_ASSERT(__atcListWindow);
  return __atcListWindow;
}

DatabaseWindow*
WidgetsUserInterface::databaseWindow() {
  Q_ASSERT(__databaseWindow);
  return __databaseWindow;
}

FlightListWindow*
WidgetsUserInterface::flightListWindow() {
  Q_ASSERT(__flightsListWindow);
  return __flightsListWindow;
}

MetarsWindow*
WidgetsUserInterface::metarsWindow() {
  Q_ASSERT(__metarsWindow);
  return __metarsWindow;
}

SettingsWindow*
WidgetsUserInterface::settingsWindow() {
  Q_ASSERT(__settingsWindow);
  return __settingsWindow;
}

VatsinatorWindow*
WidgetsUserInterface::mainWindow() {
  Q_ASSERT(__vatsinatorWindow);
  return __vatsinatorWindow;
}

void
WidgetsUserInterface::showAppRestartDialog() {
  AppRestartDialog* dialog = new AppRestartDialog();
  
  connect(dialog,       SIGNAL(accepted()),
          vApp(),       SLOT(restart()));
  connect(dialog,       SIGNAL(finished(int)),
          dialog,       SLOT(deleteLater()));
  
  dialog->show();
  dialog->raise();
  dialog->activateWindow();  
}

void
WidgetsUserInterface::fatal(const QString& _msg) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.setIcon(QMessageBox::Critical);
  
  qFatal("%s", qPrintable(_msg));
  
  msgBox.exec();
}

void
WidgetsUserInterface::warning(const QString& _msg) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.setIcon(QMessageBox::Warning);
  
  qWarning("%s", qPrintable(_msg));
  
  msgBox.exec();
}

void
WidgetsUserInterface::statusError() {
  StatusFetchErrorDialog dialog;
  dialog.exec();
}

void
WidgetsUserInterface::dataError() {
  DataFetchErrorDialog dialog;
  dialog.exec();
  
  if (dialog.clickedButton() == dialog.again()) {
    vApp()->vatsimDataHandler()->requestDataUpdate();
  }
}

void
WidgetsUserInterface::showVatsimMessage(const QString& _msg) {
  QString hash = QString::number(qHash(_msg));
  QSettings s;
  if (s.value("VatsimMessages/" % hash, false).toBool())
    return;
  
  VatsimMessageDialog* dialog = new VatsimMessageDialog(_msg);
  
  connect(dialog,       SIGNAL(finished(int)),
          dialog,       SLOT(deleteLater()));
  
  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void
WidgetsUserInterface::showDetails(const Airport* _ap) {
  AirportDetailsWindow* ap = new AirportDetailsWindow(_ap);
  ap->setAttribute(Qt::WA_DeleteOnClose);
  ap->show();
}

void
WidgetsUserInterface::showDetails(const Client* _c) {
  if (const Pilot* p = dynamic_cast<const Pilot*>(_c)) {
    FlightDetailsWindow* w = new FlightDetailsWindow(p);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
  } else if (const Controller* c = dynamic_cast<const Controller*>(_c)) {
    AtcDetailsWindow* w = new AtcDetailsWindow(c);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
  }
}

void
WidgetsUserInterface::showDetails(const Fir* _f) {
  FirDetailsWindow* w = new FirDetailsWindow(_f);
  w->setAttribute(Qt::WA_DeleteOnClose);
  w->show();
}

void
WidgetsUserInterface::showMetar(const QString& _icao) {
  metarsWindow()->show(_icao);
}

void WidgetsUserInterface::__showNewVersionDialog() {
  NewVersionDialog dialog(__vatsinatorWindow);
  dialog.exec();
}
