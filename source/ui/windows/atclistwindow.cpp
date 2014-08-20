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

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

#include "ui/userinterface.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsinatorapplication.h"

#include "atclistwindow.h"

AtcListWindow::AtcListWindow(QWidget* _parent) :
    BaseWindow(_parent) {
  setupUi(this);
  
  connect(qApp,         SIGNAL(aboutToQuit()),
          this,         SLOT(hide()));
  connect(ATCTable,     SIGNAL(doubleClicked(const QModelIndex&)),
          this,         SLOT(__handleDoubleClicked(const QModelIndex&)));
}

void
AtcListWindow::resizeEvent(QResizeEvent* _event) {
  QWidget::resizeEvent(_event);
  __resizeColumns();
}

void
AtcListWindow::showEvent(QShowEvent* _event) {
  if (auto m = ATCTable->model())
    m->deleteLater();
  
  ATCTable->setModel(vApp()->vatsimDataHandler()->controllerTableModel());
  ATCTable->hideColumn(ControllerTableModel::Button);
  __resizeColumns();
  
  BaseWindow::showEvent(_event);
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
AtcListWindow::__handleDoubleClicked(const QModelIndex& _index) {
  Q_ASSERT(qobject_cast< const ControllerTableModel* >(_index.model()));

  vApp()->userInterface()->showDetails(
    (qobject_cast<const ControllerTableModel*>(_index.model()))->staff()[_index.row()]
  );
}

