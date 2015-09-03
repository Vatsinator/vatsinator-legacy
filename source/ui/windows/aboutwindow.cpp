/*
    aboutwindow.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "storage/filemanager.h"
#include "ui/userinterface.h"
#include "vatsinatorapplication.h"
#include "config.h"

#include "aboutwindow.h"
#include "ui/about.h"

AboutWindow::AboutWindow(QWidget* parent) : BaseWindow(parent)
{
    setupUi(this);
    
    connect(qApp, &QApplication::aboutToQuit, this, &AboutWindow::close);
    connect(vApp()->resourceManager(), &ResourceManager::vatsinatorVersionChecked, this, &AboutWindow::__updateVersionStatus);
            
    AuthorsField->setHtml(trUtf8(ABOUT_TEXT));
    VersionLabel->setText(tr("Version %1").arg(VATSINATOR_VERSION));
    ChangelogField->setHtml("<pre>" % trUtf8(CHANGELOG_TEXT) % "</pre>");
    
    QFont titleFont = QApplication::font();
    titleFont.setPointSize(titleFont.pointSize() + 2);
    TitleLabel->setFont(titleFont);
}

void
AboutWindow::showEvent(QShowEvent* event)
{
    if (LicenseField->toPlainText().isEmpty()) {
        QFile file(":/about/COPYING");
        bool result = file.open(QIODevice::ReadOnly | QIODevice::Text);
        Q_ASSERT(result);
        Q_UNUSED(result);
        
        QString content = file.readAll();
        file.close();
        
        LicenseField->setHtml("<pre>" % content.toHtmlEscaped() % "</pre>");
    }
    
    BaseWindow::showEvent(event);
}

void
AboutWindow::__updateVersionStatus(ResourceManager::VersionStatus status)
{
    QPalette p = VersionStatusLabel->palette();
    
    if (status == ResourceManager::Updated) {
        p.setColor(QPalette::WindowText, Qt::darkGreen);
        VersionStatusLabel->setPalette(p);
        VersionStatusLabel->setText(tr("up-to-date", "Vatsinator version indicator"));
    } else {
        p.setColor(QPalette::WindowText, Qt::red);
        VersionStatusLabel->setPalette(p);
        VersionStatusLabel->setText(tr("outdated", "Vatsinator version indicator"));
    }
}
