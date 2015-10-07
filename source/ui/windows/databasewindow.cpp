/*
 * databasewindow.cpp
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

#include <QtWidgets>

#include "db/airlinedatabase.h"
#include "db/airportdatabase.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "databasewindow.h"

DatabaseWindow::DatabaseWindow(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    
    __updateDatabaseStatus(ResourceManager::Updated);
    
    connect(vApp()->resourceManager(), &ResourceManager::databaseStatusChanged,
            this, &DatabaseWindow::__updateDatabaseStatus);
    connect(SyncDatabaseButton, &QPushButton::clicked,
            vApp()->resourceManager(), &ResourceManager::requestDatabaseSync);
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::initialized,
            this, &DatabaseWindow::__updateNumbers);
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QDesktopWidget().screenGeometry(wui()->mainWindow())));
}

void
DatabaseWindow::__updateNumbers()
{
    DatabaseInfoLabel->setText(
        //: First part of the summary
        tr("Your local clone of Vatsinator Database contains %n airport(s)",
            "", vApp()->airportDatabase()->airports().count()
        ) % " " %
        //: Second part of the summary
        tr("and %n airlines(s).",
            "", vApp()->airlineDatabase()->airlines().size()
        )
    );
}

void
DatabaseWindow::__updateDatabaseStatus(ResourceManager::VersionStatus status)
{
    QPalette p = StatusLabel->palette();
    
    switch (status) {
        case ResourceManager::Updated:
            p.setColor(QPalette::WindowText, Qt::darkGreen);
            StatusLabel->setPalette(p);
            //: Describes database update status
            StatusLabel->setText(tr("up-to-date", "Database status indicator"));
            SyncDatabaseButton->setEnabled(true);
            break;
            
        case ResourceManager::Outdated:
            p.setColor(QPalette::WindowText, Qt::red);
            StatusLabel->setPalette(p);
            //: Describes database update status
            StatusLabel->setText(tr("outdated", "Database status indicator"));
            SyncDatabaseButton->setEnabled(true);
            break;
            
        case ResourceManager::Updating:
            p.setColor(QPalette::WindowText, Qt::darkYellow);
            StatusLabel->setPalette(p);
            //: Describes database update status
            StatusLabel->setText(tr("updating...", "Database status indicator"));
            SyncDatabaseButton->setEnabled(false);
            break;
            
        case ResourceManager::CannotUpdate:
            p.setColor(QPalette::WindowText, Qt::darkGray);
            StatusLabel->setPalette(p);
            //: Describes database update status
            StatusLabel->setText(tr("cannot update", "Database status indicator"));
            StatusLabel->setToolTip(tr("Your Vatsinator version is outdated."));
            SyncDatabaseButton->setEnabled(false);
            break;
    }
    
    UpdateDateLabel->setText(vApp()->resourceManager()->lastUpdateDate().toString(Qt::ISODate));
}
