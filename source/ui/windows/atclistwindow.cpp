/*
    atclistwindow.cpp
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

#include "ui/userinterface.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsinatorapplication.h"

#include "atclistwindow.h"

AtcListWindow::AtcListWindow(QWidget* parent) :
    BaseWindow(parent) {
  setupUi(this);
  
  connect(qApp,         SIGNAL(aboutToQuit()),
          this,         SLOT(hide()));
  connect(ATCTable,     SIGNAL(doubleClicked(const QModelIndex&)),
          this,         SLOT(__handleDoubleClicked(const QModelIndex&)));
}

void
AtcListWindow::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  __resizeColumns();
}

void
AtcListWindow::showEvent(QShowEvent* event) {
  if (auto m = ATCTable->model())
    m->deleteLater();
  
  ATCTable->setModel(vApp()->vatsimDataHandler()->controllerTableModel());
  ATCTable->hideColumn(ControllerTableModel::Button);
  __resizeColumns();
  
  BaseWindow::showEvent(event);
}

void
AtcListWindow::__resizeColumns() {
  static const int CallsignWidth = 100;
  static const int FrequencyWidth = 120;
  
  const int scrollbarWidth = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

  int spaceLeft = ATCTable->width() - CallsignWidth - FrequencyWidth - scrollbarWidth;

  ATCTable->setColumnWidth(ControllerTableModel::Callsign, CallsignWidth);
  ATCTable->setColumnWidth(ControllerTableModel::Name, spaceLeft);
}

void
AtcListWindow::__handleDoubleClicked(const QModelIndex& index) {
  Q_ASSERT(qobject_cast< const ControllerTableModel* >(index.model()));

  vApp()->userInterface()->showDetails(
    (qobject_cast<const ControllerTableModel*>(index.model()))->staff()[index.row()]
  );
}

