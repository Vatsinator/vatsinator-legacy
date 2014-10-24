/*
    flighttableview.cpp
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

#include "vatsimdata/models/flighttablemodel.h"

#include "flighttableview.h"

FlightTableView::FlightTableView(QWidget* parent) : QTableView(parent) {}

void
FlightTableView::setModel(QAbstractItemModel* model) {
  QTableView::setModel(model);
//   __resizeColumns();
}

void
FlightTableView::resizeEvent(QResizeEvent* event) {
//   __resizeColumns();
  QTableView::resizeEvent(event);
}

void
FlightTableView::__resizeColumns() {
  if (!model())
    return;
  
  int columnsVisible = 0;
  for (int i = 0; i < model()->columnCount(); ++i) {
    if (!isColumnHidden(i))
      columnsVisible += 1;
  }
  
  int w = width() / columnsVisible;
  
  for (int i = 0; i < model()->columnCount(); ++i) {
    if (!isColumnHidden(i))
      setColumnWidth(i, w);
  }
}
