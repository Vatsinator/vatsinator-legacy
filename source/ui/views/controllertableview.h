/*
    controllertableview.h
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


#ifndef CONTROLLERTABLEVIEW_H
#define CONTROLLERTABLEVIEW_H

#include <QTableView>

class ControllerTableModel;

/**
 * The ControllerTableView class is used to show all controllers in the nice,
 * pretty table. To use it properly, the ControllerTableModel instance is
 * needed.
 */
class ControllerTableView : public QTableView {
  Q_OBJECT
  
public:
  /**
   * The default constructor passes _parent_ to QTableView's.
   */
  explicit ControllerTableView(QWidget* parent = nullptr);
  
  void setModel(ControllerTableModel* model);
  
protected slots:
  void rowsInserted(const QModelIndex& parent, int start, int end) override;
  
private slots:
  /**
   * Re-sets buttons within given range.
   * If no range is given, whole view is updated.
   */
  void __updateButtons(int start = -1, int end = -1);
  
  
};

#endif // CONTROLLERTABLEVIEW_H
