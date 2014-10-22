/*
    controllertableview.cpp
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

#include "ui/buttons/clientdetailsbutton.h"
#include "ui/userinterface.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsinatorapplication.h"

#include "controllertableview.h"

ControllerTableView::ControllerTableView(QWidget* parent) :
  QTableView(parent) {}

void
ControllerTableView::setModel(ControllerTableModel* model) {
  if (this->model()) {
    disconnect(this, SLOT(__updateButtons()));
  }
  
  QTableView::setModel(model);
  
  __updateButtons();
  
  connect(model,        SIGNAL(sorted()),
          this,         SLOT(__updateButtons()));
}

void
ControllerTableView::rowsInserted(const QModelIndex& parent, int start, int end) {
  QTableView::rowsInserted(parent, start, end);
  __updateButtons(start, end + 1);
}

void
ControllerTableView::__updateButtons(int start, int end) {
  Q_ASSERT(model());
  const ControllerTableModel* cModel = qobject_cast<const ControllerTableModel*>(model());
  Q_ASSERT(cModel);
  
  if (start == -1)
    start = 0;
  
  if (end == -1)
    end = cModel->rowCount();
  
  for (int i = start; i < end; ++i) {
    ClientDetailsButton* dButton = new ClientDetailsButton(cModel->staff()[i]);
    connect(dButton,                    SIGNAL(clicked(const Client*)),
            vApp()->userInterface(),    SLOT(showDetails(const Client*)));
    setIndexWidget(cModel->index(i, ControllerTableModel::Button), dButton);
  }
}
