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

#include <QtWidgets>

#include "events/decisionevent.h"
#include "network/resourcemanager.h"
#include "storage/settingsmanager.h"
#include "ui/dialogs/apprestartdialog.h"
#include "ui/dialogs/datafetcherrordialog.h"
#include "ui/dialogs/letsendstatsdialog.h"
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
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"
#include "vatsinatorapplication.h"

#include "widgetsuserinterface.h"

WidgetsUserInterface::WidgetsUserInterface(QObject* parent):
    UserInterface(parent) {}

WidgetsUserInterface::~WidgetsUserInterface()
{
    delete __aboutWindow;
    delete __metarsWindow;
    delete __databaseWindow;
    delete __atcListWindow;
    delete __flightListWindow;
    delete __settingsWindow;
}

void
WidgetsUserInterface::initialize()
{
    __aboutWindow = new AboutWindow();
    __metarsWindow = new MetarsWindow();
    __databaseWindow = new DatabaseWindow();
    __atcListWindow = new AtcListWindow();
    __flightListWindow = new FlightListWindow();
    __settingsWindow = new SettingsWindow();
    __vatsinatorWindow = new VatsinatorWindow();
    
    connect(vApp()->resourceManager(), &ResourceManager::outdated,
            this, &WidgetsUserInterface::__showNewVersionDialog);
    
    emit initialized();
    
    mainWindow()->show();
}

AboutWindow*
WidgetsUserInterface::aboutWindow()
{
    Q_ASSERT(__aboutWindow);
    return __aboutWindow;
}

AtcListWindow*
WidgetsUserInterface::atcListWindow()
{
    Q_ASSERT(__atcListWindow);
    return __atcListWindow;
}

DatabaseWindow*
WidgetsUserInterface::databaseWindow()
{
    Q_ASSERT(__databaseWindow);
    return __databaseWindow;
}

FlightListWindow*
WidgetsUserInterface::flightListWindow()
{
    Q_ASSERT(__flightListWindow);
    return __flightListWindow;
}

MetarsWindow*
WidgetsUserInterface::metarsWindow()
{
    Q_ASSERT(__metarsWindow);
    return __metarsWindow;
}

SettingsWindow*
WidgetsUserInterface::settingsWindow()
{
    Q_ASSERT(__settingsWindow);
    return __settingsWindow;
}

VatsinatorWindow*
WidgetsUserInterface::mainWindow()
{
    Q_ASSERT(__vatsinatorWindow);
    return __vatsinatorWindow;
}

void
WidgetsUserInterface::showAppRestartDialog()
{
    AppRestartDialog* dialog = new AppRestartDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &AppRestartDialog::accepted, vApp(), &VatsinatorApplication::restart);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

void
WidgetsUserInterface::showClientDetails(const Client* client)
{
    if (const Pilot* p = qobject_cast<const Pilot*>(client)) {
        FlightDetailsWindow* w = new FlightDetailsWindow(p);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    } else if (const Controller* c = qobject_cast<const Controller*>(client)) {
        AtcDetailsWindow* w = new AtcDetailsWindow(c);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    }
}

void
WidgetsUserInterface::showAirportDetails(const Airport* airport)
{
    AirportDetailsWindow* ap = new AirportDetailsWindow(airport);
    ap->setAttribute(Qt::WA_DeleteOnClose);
    ap->show();
}

void
WidgetsUserInterface::showFirDetails(const Fir* fir)
{
    FirDetailsWindow* w = new FirDetailsWindow(fir);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void
WidgetsUserInterface::showMetar(const QString& icao)
{
    metarsWindow()->show(icao);
}

void
WidgetsUserInterface::fatal(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Critical);
    
    qFatal("%s", qPrintable(message));
    
    msgBox.exec();
}

void
WidgetsUserInterface::warning(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Warning);
    
    qWarning("%s", qPrintable(message));
    
    msgBox.exec();
}

void
WidgetsUserInterface::showStatsDialog()
{
    LetSendStatsDialog* dialog = new LetSendStatsDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    
    connect(dialog, &LetSendStatsDialog::accepted, []() {
        DecisionEvent* e = new DecisionEvent("statistics", DecisionEvent::Accepted);
        QCoreApplication::postEvent(vApp(), e);
    });
    
    connect(dialog, &LetSendStatsDialog::rejected, []() {
        DecisionEvent* e = new DecisionEvent("statistics", DecisionEvent::Declined);
        QCoreApplication::postEvent(vApp(), e);
    });
    
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

void
WidgetsUserInterface::ensureMainWindowIsActive()
{
    mainWindow()->show();
    mainWindow()->activateWindow();
}

void
WidgetsUserInterface::vatsimEvent(VatsimEvent* event)
{
    switch (event->type()) {
        case VatsimEvent::Message: {
            QString hash = QString::number(qHash(event->message()));
            QSettings s;
            
            if (s.value("VatsimMessages/" % hash, false).toBool())
                return;
            
            VatsimMessageDialog* dialog = new VatsimMessageDialog(event->message());
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->show();
            dialog->raise();
            dialog->activateWindow();
            break;
        }
        
        case VatsimEvent::StatusError: {
            StatusFetchErrorDialog dialog;
            dialog.exec();
            break;
        }
        
        case VatsimEvent::DataError: {
            DataFetchErrorDialog dialog;
            dialog.exec();
            
            if (dialog.clickedButton() == dialog.again())
                vApp()->vatsimDataHandler()->requestDataUpdate();
            
            break;
        }
    }
}

void
WidgetsUserInterface::__showNewVersionDialog()
{
    NewVersionDialog dialog(__vatsinatorWindow);
    dialog.exec();
}
