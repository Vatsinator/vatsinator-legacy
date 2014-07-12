/*
    aboutwindow.cpp
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

#include <QtWidgets>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "storage/filemanager.h"
#include "ui/userinterface.h"
#include "config.h"

#include "aboutwindow.h"
#include "ui/about.h"
#include "ui/license.h"

AboutWindow::AboutWindow(QWidget* _parent) : BaseWindow(_parent) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));
  connect(ResourceManager::getSingletonPtr(),
                SIGNAL(vatsinatorVersionChecked(ResourceManager::VersionStatus)),
          this,
                SLOT(__updateVersionStatus(ResourceManager::VersionStatus)));
  
  AuthorsField->setHtml(trUtf8(ABOUT_TEXT));
  LicenseField->setHtml("<pre>" % trUtf8(LICENSE_TEXT) % "</pre>");
  VersionLabel->setText(tr("Version %1").arg(VATSINATOR_VERSION));
  ChangelogField->setHtml("<pre>" % trUtf8(CHANGELOG_TEXT) % "</pre>");
  
  QFont titleFont = QApplication::font();
  titleFont.setPointSize(titleFont.pointSize() + 2);
  TitleLabel->setFont(titleFont);
}

void
AboutWindow::__updateVersionStatus(ResourceManager::VersionStatus _status) {
  QPalette p = VersionStatusLabel->palette();
  if (_status == ResourceManager::Updated) {
    p.setColor(QPalette::WindowText, Qt::darkGreen);
    VersionStatusLabel->setPalette(p);
    VersionStatusLabel->setText(tr("up-to-date", "Vatsinator version indicator"));
  } else {
    p.setColor(QPalette::WindowText, Qt::red);
    VersionStatusLabel->setPalette(p);
    VersionStatusLabel->setText(tr("outdated", "Vatsinator version indicator"));
  }
}


