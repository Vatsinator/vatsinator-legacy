/*
    userinterface.cpp
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

#include <QtWidgets>

#include "events/notificationevent.h"
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
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsinatorapplication.h"

#include "userinterface.h"

UserInterface::UserInterface(QObject* _parent) :
    QObject(_parent),
    __aboutWindow(nullptr),
    __metarsWindow(nullptr),
    __databaseWindow(nullptr),
    __atcListWindow(nullptr),
    __flightsListWindow(nullptr),
    __settingsWindow(nullptr),
    __vatsinatorWindow(nullptr) {}

UserInterface::~UserInterface() {
  delete __vatsinatorWindow;
  delete __aboutWindow;
  delete __databaseWindow;
  delete __flightsListWindow;
  delete __atcListWindow;
  delete __settingsWindow;
  delete __metarsWindow;
}

void
UserInterface::init() {
  __aboutWindow = new AboutWindow();
  __metarsWindow = new MetarsWindow();
  __databaseWindow = new DatabaseWindow();
  __atcListWindow = new AtcListWindow();
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

AboutWindow*
UserInterface::aboutWindow() {
  Q_ASSERT(__aboutWindow);
  return __aboutWindow;
}

AtcListWindow*
UserInterface::atcListWindow() {
  Q_ASSERT(__atcListWindow);
  return __atcListWindow;
}

DatabaseWindow*
UserInterface::databaseWindow() {
  Q_ASSERT(__databaseWindow);
  return __databaseWindow;
}

FlightListWindow*
UserInterface::flightListWindow() {
  Q_ASSERT(__flightsListWindow);
  return __flightsListWindow;
}

MetarsWindow*
UserInterface::metarsWindow() {
  Q_ASSERT(__metarsWindow);
  return __metarsWindow;
}

SettingsWindow*
UserInterface::settingsWindow() {
  Q_ASSERT(__settingsWindow);
  return __settingsWindow;
}

VatsinatorWindow*
UserInterface::mainWindow() {
  Q_ASSERT(__vatsinatorWindow);
  return __vatsinatorWindow;
}

bool UserInterface::event(QEvent* _e) {
  switch (_e->type()) {
    case Event::Notification:
      return notificationEvent(dynamic_cast<NotificationEvent*>(_e));
    default:
      return QObject::event(_e);
  }
}

void
UserInterface::fatal(const QString& _msg) {
  QMessageBox msgBox;
  msgBox.setText(_msg);
  msgBox.setIcon(QMessageBox::Critical);
  
  VatsinatorApplication::log(qPrintable(_msg));
  
  msgBox.exec();
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
  AirportDetailsWindow* ap = new AirportDetailsWindow(_ap);
  ap->setAttribute(Qt::WA_DeleteOnClose);
  ap->show();
}

void
UserInterface::showDetailsWindow(const Client* _c) {
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
UserInterface::showDetailsWindow(const Fir* _f) {
  FirDetailsWindow* w = new FirDetailsWindow(_f);
  w->setAttribute(Qt::WA_DeleteOnClose);
  w->show();
}

bool
UserInterface::notificationEvent(NotificationEvent* _event) {
  switch (_event->gravity()) {
    case NotificationEvent::Fatal:
      fatal(_event->message());
      QCoreApplication::exit(1);
      break;
      
    case NotificationEvent::Warning:
      warning(_event->message());
      break;
  }
  
  return true;
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
