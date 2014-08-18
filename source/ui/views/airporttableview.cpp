/*
    airporttableview.cpp
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

#include "ui/buttons/airportdetailsbutton.h"
#include "ui/userinterface.h"
#include "vatsimdata/models/airporttablemodel.h"
#include "vatsinatorapplication.h"

#include "airporttableview.h"

AirportTableView::AirportTableView(QWidget* _parent) :
    QTableView(_parent) {}

void
AirportTableView::setModel(AirportTableModel* _model) {
  QTableView::setModel(_model);
  __updateButtons();
}

void
AirportTableView::rowsInserted(const QModelIndex& _parent, int _start, int _end) {
  QTableView::rowsInserted(_parent, _start, _end);
  __updateButtons(_start, _end + 1);
}

void
AirportTableView::__updateButtons(int _start, int _end) {
  const AirportTableModel* apModel = qobject_cast<const AirportTableModel*>(model());
  Q_ASSERT(apModel);
  
  if (_start == -1)
    _start = 0;
  
  if (_end == -1)
    _end = apModel->rowCount();

  for (int i = _start; i < _end; ++i) {
    AirportDetailsButton* dButton = new AirportDetailsButton(apModel->airports()[i]);
    connect(dButton,                    SIGNAL(clicked(const Airport*)),
            vApp()->userInterface(),    SLOT(showDetails(const Airport*)));
    setIndexWidget(apModel->index(i, AirportTableModel::Button), dButton);
  }
}
