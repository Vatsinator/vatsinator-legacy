/*
    controllertableview.h
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


#ifndef CONTROLLERTABLEVIEW_H
#define CONTROLLERTABLEVIEW_H

#include <QTableView>

class ControllerTableModel;

class ControllerTableView : public QTableView {
  
  /*
   * This class is used to show all controllers in the nice, pretty
   * table. To use it properly, the ControllerTableModel instance is
   * needed.
   */
  
  Q_OBJECT
  
public:
  explicit ControllerTableView(QWidget* = 0);
  
  void setModel(ControllerTableModel*);
  
protected slots:
  void rowsInserted(const QModelIndex&, int, int);
  
private slots:
  /**
   * Re-sets buttons within given range.
   * If no range is given, whole view is updated.
   */
  void __updateButtons(int = -1, int = -1);
  
  
};

#endif // CONTROLLERTABLEVIEW_H
