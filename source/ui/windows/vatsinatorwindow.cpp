/*
 * vatsinatorwindow.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include <QtGui>

#ifndef NO_DEBUG
# include "debugging/debugwindow.h"
#endif

#include "modules/homelocation.h"
#include "ui/windows/aboutwindow.h"
#include "ui/windows/atclistwindow.h"
#include "ui/windows/databasewindow.h"
#include "ui/windows/flightlistwindow.h"
#include "ui/windows/metarswindow.h"
#include "ui/windows/settingswindow.h"
#include "ui/userinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "vatsinatorwindow.h"
#include "defines.h"

VatsinatorWindow::VatsinatorWindow(QWidget* _parent) :
    QMainWindow(_parent) {
  setupUi(this);

  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(close()));
  
  connect(ActionExit,                                   SIGNAL(triggered()),
          qApp,                                         SLOT(quit()));
  connect(ActionAbout,                                  SIGNAL(triggered()),
          vApp()->userInterface()->aboutWindow(),       SLOT(show()));
  connect(ActionMetar,                                  SIGNAL(triggered()),
          vApp()->userInterface()->metarsWindow(),      SLOT(show()));
  connect(ActionDatabase,                               SIGNAL(triggered()),
          vApp()->userInterface()->databaseWindow(),    SLOT(show()));
  connect(ActionRefresh,                                SIGNAL(triggered()),
          VatsimDataHandler::getSingletonPtr(),         SLOT(requestDataUpdate()));
  connect(ActionPreferences,                            SIGNAL(triggered()),
          vApp()->userInterface()->settingsWindow(),    SLOT(show()));
  connect(ActionFlightList,                             SIGNAL(triggered()),
          vApp()->userInterface()->flightListWindow(),  SLOT(show()));
  connect(ActionATCList,                                SIGNAL(triggered()),
          vApp()->userInterface()->atcListWindow(),     SLOT(show()));
//   connect(ActionHomeLocation,                       SIGNAL(triggered()),
//           HomeLocation::getSingletonPtr(),          SLOT(showOnMap()));
  
  connect(VatsimDataHandler::getSingletonPtr(),         SIGNAL(vatsimDataDownloading()),
          this,                                         SLOT(__dataDownloading()));
  connect(VatsimDataHandler::getSingletonPtr(),         SIGNAL(vatsimStatusUpdated()),
          this,                                         SLOT(__statusUpdated()));
  connect(VatsimDataHandler::getSingletonPtr(),         SIGNAL(vatsimStatusError()),
          this,                                         SLOT(__dataCorrupted()));
  connect(VatsimDataHandler::getSingletonPtr(),         SIGNAL(vatsimDataUpdated()),
          this,                                         SLOT(__dataUpdated()));
  connect(VatsimDataHandler::getSingletonPtr(),         SIGNAL(vatsimDataError()),
          this,                                         SLOT(__dataCorrupted()));
  connect(VatsimDataHandler::getSingletonPtr(),         SIGNAL(vatsimStatusUpdated()),
          this,                                         SLOT(__enableRefreshAction()));
  
#ifdef Q_OS_DARWIN
  /* On Mac set main manu name to "Menu" in order not to have two
     "Vatsinator"s on the menubar. */
  MenuVatsinator->setTitle(tr("&Menu"));
#endif
  
  __statusBox = new QLabel();
  __statusBox->setIndent(5);
  
  __progressBar = new QProgressBar();
  __progressBar->setValue(0);
  __progressBar->setTextVisible(true);
  
  Replaceable->addWidgets({__statusBox, __progressBar});

#ifndef NO_DEBUG
  MenuHelp->addSeparator();

  QAction* debugAction = new QAction("Debug...", this);

  connect(debugAction,                    SIGNAL(triggered()),
          DebugWindow::getSingletonPtr(), SLOT(show()));

  MenuHelp->addAction(debugAction);
#endif
  
  statusBarUpdate();
}

void
VatsinatorWindow::statusBarUpdate(const QString& _message, const QPalette& palette) {
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
  
  __statusBox->setPalette(palette);
}

void
VatsinatorWindow::infoBarUpdate() {
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
VatsinatorWindow::positionBoxUpdate(qreal _lon, qreal _lat) {
  PositionBox->setText(QString("%1 %2 %3 %4").arg(
    _lat > 0 ? "N" : "S",
    QString::number(qAbs(_lat), 'g', 6),
    _lon < 0 ? "W" : "E",
    QString::number(qAbs(_lon), 'g', 6)
  ));
}

void
VatsinatorWindow::closeEvent(QCloseEvent*) {
  __storeWindowGeometry();
  qApp->quit();
}

void
VatsinatorWindow::showEvent(QShowEvent*) {
  __restoreWindowGeometry();
}

void
VatsinatorWindow::__storeWindowGeometry() {
   QSettings settings;

  settings.beginGroup("MainWindow");

  settings.setValue("geometry", saveGeometry());
  settings.setValue("savestate", saveState());
  settings.setValue("maximized", isMaximized());

  if (!isMaximized()) {
    settings.setValue("position", pos());
    settings.setValue("size", size());
  }

  settings.endGroup();
}

void
VatsinatorWindow::__restoreWindowGeometry() {
  QSettings settings;

  settings.beginGroup("MainWindow");
  
  if (settings.contains("geometry")) { /* Restore saved geometry */
    restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
    restoreState(settings.value("savestate", saveState()).toByteArray());
    move(settings.value("position", pos()).toPoint());
    resize(settings.value("size", size()).toSize());
    
    if (settings.value( "maximized", isMaximized()).toBool())
      showMaximized();
  } else { /* Place the window in the middle of the screen */
    setGeometry(
      QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        size(),
        QDesktopWidget().screenGeometry()
      )
    );
  }
  
  EnableAutoUpdatesAction->setChecked(settings.value("autoUpdatesEnabled", true).toBool());
  settings.endGroup();
}

void
VatsinatorWindow::__dataDownloading() {
  Replaceable->setCurrentWidget(__progressBar);
}

void
VatsinatorWindow::__statusUpdated() {
  statusBarUpdate();
}

void
VatsinatorWindow::__dataUpdated() {
  infoBarUpdate();
  statusBarUpdate();
  Replaceable->setCurrentWidget(__statusBox);
}

void
VatsinatorWindow::__dataCorrupted() {
  Replaceable->setCurrentWidget(__statusBox);
  QPalette palette = __statusBox->palette();
  palette.setColor(__statusBox->foregroundRole(), Qt::red);
  statusBarUpdate("", palette);
}

void
VatsinatorWindow::__enableRefreshAction() {
  ActionRefresh->setEnabled(true);
}
