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

#include <functional>
#include <QtWidgets>

#include "events/mouselonlatevent.h"
#include "network/plaintextdownloader.h"
#include "ui/widgets/mapinfowidget.h"
#include "ui/windows/aboutwindow.h"
#include "ui/windows/atclistwindow.h"
#include "ui/windows/databasewindow.h"
#include "ui/windows/flightlistwindow.h"
#include "ui/windows/metarswindow.h"
#include "ui/windows/settingswindow.h"
#include "ui/widgetsuserinterface.h"
#include "ui/vatsinatorstyle.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "vatsinatorwindow.h"

VatsinatorWindow::VatsinatorWindow(QWidget* parent) :
    QMainWindow(parent)
{
    setupUi(this);
            
    Q_ASSERT(wui());
    
    connect(ActionExit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ActionAbout, &QAction::triggered, wui()->aboutWindow(), &AboutWindow::show);
    connect(ActionMetar, &QAction::triggered, wui()->metarsWindow(), &QWidget::show);
    connect(ActionDatabase, &QAction::triggered, wui()->databaseWindow(), &DatabaseWindow::show);
    connect(ActionRefresh, &QAction::triggered, vApp()->vatsimDataHandler(), &VatsimDataHandler::requestDataUpdate);
    connect(ActionPreferences, &QAction::triggered, wui()->settingsWindow(), &SettingsWindow::show);
    connect(ActionFlightList, &QAction::triggered, wui()->flightListWindow(), &FlightListWindow::show);
    connect(ActionATCList, &QAction::triggered, wui()->atcListWindow(), &AtcListWindow::show);
    // TODO
    //   connect(ActionHomeLocation,                       SIGNAL(triggered()),
    //           HomeLocation::getSingletonPtr(),          SLOT(showOnMap()));
    
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimStatusUpdated,
            std::bind(&QAction::setEnabled, ActionRefresh, true));
    
#ifdef Q_OS_MAC
    /* On Mac set main manu name to "Menu" in order not to have two
       "Vatsinators" on the menubar. */
    MenuVatsinator->setTitle(tr("&Menu"));
    
    /* Set small font for the bottom status bar */
    VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
#endif
    
    __mapInfo = new MapInfoWidget;
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addItem(new QSpacerItem(0, 20));
    layout->addWidget(__mapInfo);
    layout->setStretch(0, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    MapDisplay->setLayout(layout);
    
    
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimDataDownloading,
            std::bind(&MapInfoWidget::setUpdatedVisible, __mapInfo, false));
    
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimDataUpdated, [this]() {
        __mapInfo->setUpdatedVisible(true);
        __mapInfo->setUpdated(vApp()->vatsimDataHandler()->dateDataUpdated());
    });
}

void
VatsinatorWindow::customEvent(QEvent* event)
{
    if (event->type() == Event::MouseLonLat)
        mouseLonLatMoveEvent(static_cast<MouseLonLatEvent*>(event));
}

void
VatsinatorWindow::closeEvent(QCloseEvent*)
{
    __storeWindowGeometry();
    qApp->quit();
}

void
VatsinatorWindow::showEvent(QShowEvent*)
{
    __restoreWindowGeometry();
}

void
VatsinatorWindow::mouseLonLatMoveEvent(MouseLonLatEvent* event)
{
    __mapInfo->setPosition(event->lonLat());
}

void
VatsinatorWindow::__storeWindowGeometry()
{
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
VatsinatorWindow::__restoreWindowGeometry()
{
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
