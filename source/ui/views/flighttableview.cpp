/*
    flighttableview.cpp
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
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "flighttableview.h"

FlightTableView::FlightTableView(QWidget* _parent) :
    QTableView(_parent) {}

void
FlightTableView::setModel(FlightTableModel* _model) {
  if (this->model()) {
    disconnect(this, SLOT(__updateButtons()));
  }
  
  QTableView::setModel(_model);
  
  __updateButtons();
  
  connect(_model,    SIGNAL(sorted()),
          this,      SLOT(__updateButtons()));
}

void
FlightTableView::rowsInserted(const QModelIndex& _parent, int _start, int _end) {
  QTableView::rowsInserted(_parent, _start, _end);
  __updateButtons(_start, _end + 1);
}

void
FlightTableView::__updateButtons(int _start, int _end) {
  Q_ASSERT(model());
  const FlightTableModel* fModel = qobject_cast<const FlightTableModel*>(model());
  Q_ASSERT(fModel);
  
  if (_start == -1)
    _start = 0;
  
  if (_end == -1)
    _end = fModel->rowCount();
  
  for (int i = _start; i < _end; ++i) {
    if (fModel->flights()[i]->isPrefiledOnly())
      continue;
    
    ClientDetailsButton* dButton = new ClientDetailsButton(fModel->flights()[i]);
    connect(dButton,                    SIGNAL(clicked(const Client*)),
            vApp()->userInterface(),    SLOT(showDetails(const Client*)));
    setIndexWidget(fModel->index(i, FlightTableModel::Button), dButton);
  }
}


