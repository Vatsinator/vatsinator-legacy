/*
    atclistwindow.cpp
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

#include <QtGui>

#include "ui/userinterface.h"

#include "ui/windows/atcdetailswindow.h"

#include "vatsimdata/client/controller.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsimdata/models/controllertablemodel.h"

#include "atclistwindow.h"
#include "defines.h"

AtcListWindow::AtcListWindow(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);
  UserInterface::setWindowPosition(this);
  ATCTable->setModel(VatsimDataHandler::getSingleton().atcModel());
  ATCTable->hideColumn(ControllerTableModel::Button);
  __setColumnsWidths();

  connect(ATCTable, SIGNAL(doubleClicked(const QModelIndex&)),
          this,     SLOT(__handleDoubleClicked(const QModelIndex&)));
}

void
AtcListWindow::resizeEvent(QResizeEvent* _event) {
  QWidget::resizeEvent(_event);
  __setColumnsWidths();
}

void
AtcListWindow::__setColumnsWidths() {
  static const int CALLSIGN_SIZE = 100;
  static const int FREQUENCY_SIZE = 120;
  static const int SCROLLBAR_SIZE = 30;

  int spaceLeft = ATCTable->width() - CALLSIGN_SIZE - FREQUENCY_SIZE - SCROLLBAR_SIZE;

  ATCTable->setColumnWidth(ControllerTableModel::Callsign, CALLSIGN_SIZE);
  ATCTable->setColumnWidth(ControllerTableModel::Name, spaceLeft);
}

void
AtcListWindow::__handleDoubleClicked(const QModelIndex& _index) {
  Q_ASSERT(qobject_cast< const ControllerTableModel* >(_index.model()));

  AtcDetailsWindow::getSingleton().show(
    (qobject_cast< const ControllerTableModel* >(_index.model()))->staff()[_index.row()]
  );
}

