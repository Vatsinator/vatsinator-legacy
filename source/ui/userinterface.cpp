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
#include "debugging/debugwindow.h"
#endif

#include "network/resourcemanager.h"

#include "modules/homelocation.h"

#include "storage/settingsmanager.h"

#include "ui/dialogs/newversiondialog.h"

#include "ui/windows/aboutwindow.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/atclistwindow.h"
#include "ui/windows/firdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "ui/windows/flightlistwindow.h"
#include "ui/windows/metarswindow.h"
#include "ui/windows/settingswindow.h"

#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/client.h"

#include "vatsinatorapplication.h"

#include "userinterface.h"
#include "defines.h"

QPoint UserInterface::__initialPoint(0, 0);

UserInterface::UserInterface(QWidget* _parent) :
    QMainWindow(_parent),
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
    __settingsWindow(new SettingsWindow()) {
  __setupWindow();
  setWindowPosition(this);
  __restoreWindowGeometry();

  connect(ActionExit,                               SIGNAL(triggered()),
          this,                                     SLOT(quit()));
  connect(ActionAbout,                              SIGNAL(triggered()),
          __aboutWindow,                            SLOT(show()));
  connect(ActionMetar,                              SIGNAL(triggered()),
          __metarsWindow,                           SLOT(show()));
  connect(ActionRefresh,                            SIGNAL(triggered()),
          VatsinatorApplication::getSingletonPtr(), SLOT(refreshData()));
  connect(ActionPreferences,                        SIGNAL(triggered()),
          __settingsWindow,                         SLOT(show()));
  connect(ActionFlightList,                         SIGNAL(triggered()),
          __flightsListWindow,                      SLOT(show()));
  connect(ActionATCList,                            SIGNAL(triggered()),
          __atcListWindow,                          SLOT(show()));
  connect(EnableAutoUpdatesAction,                  SIGNAL(toggled(bool)),
          this,                                     SIGNAL(autoUpdatesEnabled(bool)));
  connect(ActionHomeLocation,                       SIGNAL(triggered()),
	  HomeLocation::getSingletonPtr(),          SLOT(showOnMap()));
  connect(VatsinatorApplication::getSingletonPtr(), SIGNAL(dataDownloading()),
          this,                                     SLOT(__dataDownloading()));
  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(vatsimStatusUpdated()),
          this,                                     SLOT(__statusUpdated()));
  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(vatsimDataUpdated()),
          this,                                     SLOT(__dataUpdated()));
  connect(VatsimDataHandler::getSingletonPtr(),     SIGNAL(dataCorrupted()),
          this,                                     SLOT(__fetchError()));
  connect(ResourceManager::getSingletonPtr(),       SIGNAL(outdated()),
          this,                                     SLOT(__showNewVersionDialog()));

  statusBarUpdate();
}

UserInterface::~UserInterface() {
  hideAllWindows();
  __storeWindowGeometry();

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
UserInterface::statusBarUpdate(const QString& _message) {
  if (_message.isEmpty()) {
    if (VatsimDataHandler::getSingleton().dateDataUpdated().isNull())
      __statusBox->setText(tr("Last update: never"));
    else
      __statusBox->setText(tr("Last update: %1 UTC").arg(
          VatsimDataHandler::getSingleton().dateDataUpdated().toString("dd MMM yyyy, hh:mm")
        ));
  } else {
    __statusBox->setText(_message);
  }
}

void
UserInterface::toggleStatusBar() {
  Replaceable->next();
}

void
UserInterface::infoBarUpdate() {
  VatsimDataHandler& data = VatsimDataHandler::getSingleton();
  
  ClientsBox->setText(tr(
      "Clients: %1 (%2 pilots, %3 ATCs, %4 observers)").arg(
          QString::number(data.clientCount()),
          QString::number(data.pilotCount()),
          QString::number(data.atcCount()),
          QString::number(data.obsCount())
        )
   );
}

void
UserInterface::setWindowPosition(QWidget* _window) {
  QRect frect = _window->frameGeometry();
  frect.moveCenter(QDesktopWidget().availableGeometry(UserInterface::__getInitialPoint()).center());
  _window->move(frect.topLeft());
}

void
UserInterface::quit() {
  VatsinatorApplication::getSingleton().quit();
}

void
UserInterface::hideAllWindows() {
  if (__aboutWindow->isVisible())
    __aboutWindow->hide();

  if (__airportDetailsWindow->isVisible())
    __airportDetailsWindow->hide();

  if (__firDetailsWindow->isVisible())
    __firDetailsWindow->hide();

  if (__flightDetailsWindow->isVisible())
    __flightDetailsWindow->hide();

  if (__flightsListWindow->isVisible())
    __flightsListWindow->hide();

  if (__atcDetailsWindow->isVisible())
    __atcDetailsWindow->hide();

  if (__atcListWindow->isVisible())
    __atcListWindow->hide();

  if (__metarsWindow->isVisible())
    __metarsWindow->hide();

#ifndef NO_DEBUG

  if (__debugWindow->isVisible())
    __debugWindow->hide();

#endif
}

void
UserInterface::closeEvent(QCloseEvent* _event) {
  hideAllWindows();
  _event->accept();
}

void
UserInterface::__setupWindow() {
  setupUi(this);
  
#ifdef Q_OS_DARWIN
  /* On Mac set main manu name to "Menu" in order not to have two
     "Vatsinator"s on the menubar. */
  MenuVatsinator->setTitle(tr("&Menu"));
#endif
  
  __statusBox = new QLabel();
  __statusBox->setMinimumSize(QSize(250, 13));
  __statusBox->setMaximumSize(QSize(250, 13));
  __statusBox->setIndent(5);
  
  __progressBar = new QProgressBar();
  __progressBar->setMinimumSize(QSize(250, 13));
  __progressBar->setMaximumSize(QSize(250, 13));
  __progressBar->setValue(0);
  __progressBar->setTextVisible(true);
  
  Replaceable->addWidgets({__statusBox, __progressBar});

#ifndef NO_DEBUG
  MenuHelp->addSeparator();

  QAction* debugAction = new QAction("Debug...", this);

  connect(debugAction,    SIGNAL(triggered()),
          __debugWindow,  SLOT(show()));

  MenuHelp->addAction(debugAction);
#endif
}

void
UserInterface::__storeWindowGeometry() {
  QSettings settings;

  settings.beginGroup("MainWindow");

  settings.setValue("geometry", saveGeometry());
  settings.setValue("savestate", saveState());
  settings.setValue("maximized", isMaximized());

  if (!isMaximized()) {
    settings.setValue("position", pos());
    settings.setValue("size", size());
  }
  
  settings.setValue("autoUpdatesEnabled", autoUpdatesEnabled());

  settings.endGroup();
}

void
UserInterface::__restoreWindowGeometry() {
  QSettings settings;

  settings.beginGroup("MainWindow");
  restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
  restoreState(settings.value("savestate", saveState()).toByteArray());
  move(settings.value("position", pos()).toPoint());
  resize(settings.value("size", size()).toSize());

  if ( settings.value( "maximized", isMaximized()).toBool() )
    showMaximized();
  
  EnableAutoUpdatesAction->setChecked(settings.value("autoUpdatesEnabled", true).toBool());

  settings.endGroup();
}

const QPoint &
UserInterface::__getInitialPoint() {
  if (__initialPoint.isNull()) {
    QSettings settings;
    
    settings.beginGroup("MainWindow");
    __initialPoint = settings.value("position", QPoint(1, 1)).toPoint();
    
    settings.endGroup();
  }
  
  return __initialPoint;
}

void
UserInterface::__dataDownloading() {
  Replaceable->setCurrentWidget(__progressBar);
}

void
UserInterface::__statusUpdated() {
  statusBarUpdate();
}

void
UserInterface::__dataUpdated() {
  infoBarUpdate();
  statusBarUpdate();
  Replaceable->setCurrentWidget(__statusBox);
}

void
UserInterface::__fetchError() {
  Replaceable->setCurrentWidget(__statusBox);
  
  QMessageBox decision;
  decision.setText(tr("Vatsinator was unable to fetch Vatsim's data file."));
  decision.setInformativeText(tr("What do you want to do with that?"));
  QPushButton* againButton = decision.addButton(tr("Try again"), QMessageBox::ActionRole);
  decision.addButton(tr("Keep current data"), QMessageBox::RejectRole);
  decision.setIcon(QMessageBox::Warning);
  
  decision.exec();
  
  if (decision.clickedButton() == againButton) {
    VatsinatorApplication::getSingleton().refreshData();
  } else {
    statusBarUpdate(tr("Data outdated!"));
  }
}

void
UserInterface::__showNewVersionDialog() {
  NewVersionDialog dialog(this);
  dialog.exec();
}
