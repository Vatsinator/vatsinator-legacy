/*
    settingswindow.cpp
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

#include "storage/languagemanager.h"
#include "storage/settingsmanager.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/widgets/mapwidget.h"
#include "ui/userinterface.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/pages/networkpage.h"
#include "ui/pages/viewpage.h"
#include "ui/pages/mappage.h"
#include "vatsinatorapplication.h"

#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget* parent) :
    BaseWindow(parent) {
  setupUi(this);
  
  {
    MiscellaneousPage* p = new MiscellaneousPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
  }
  {
    NetworkPage* p = new NetworkPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
  }
  {
    ViewPage* p = new ViewPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
  }
  {
    MapPage* p = new MapPage();
    __addPage(p->listElement(), p->listIcon(), p);
    vApp()->settingsManager()->addPage(p);
  }
  
  connect(qApp,                                 SIGNAL(aboutToQuit()),
          this,                                 SLOT(hide()));
  connect(OKCancelButtonBox,                    SIGNAL(clicked(QAbstractButton*)),
          this,                                 SLOT(__handleButton(QAbstractButton*)));
  connect(this,                                 SIGNAL(settingsApplied()),
          vApp()->settingsManager(),            SLOT(saveSettings()));
  connect(this,                                 SIGNAL(restoreDefaults()),
          vApp()->settingsManager(),            SLOT(restoreDefaults()));
}

void
SettingsWindow::__addPage(const QString& element, const QString& icon, QWidget* page) {
  QListWidgetItem *item = new QListWidgetItem(CategoryList);
  
  QIcon listIcon(icon);
  item->setIcon(listIcon);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  
  item->setText(element);
  
  SwappingWidget->addWidget(page);
  
  if (CategoryList->count() == 1)
    item->setSelected(true);
}

void
SettingsWindow::__handleButton(QAbstractButton* button) {
  if (OKCancelButtonBox->button(QDialogButtonBox::RestoreDefaults) == button)
    emit restoreDefaults();
  else if (OKCancelButtonBox->button(QDialogButtonBox::Apply) == button
        || OKCancelButtonBox->button(QDialogButtonBox::Ok) == button)
    emit settingsApplied();
}

