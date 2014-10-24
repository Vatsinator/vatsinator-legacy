/*
    flighttableview.h
    Copyright (C) 2012-2014  michal@garapich.pl

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


#ifndef FLIGHTTABLEVIEW_H
#define FLIGHTTABLEVIEW_H

#include <QTableView>

class FlightTableModel;

/**
 * The FlightTableView class is used to represent all of the flights in the
 * table. It automatically resizes columns to be readable.
 */
class FlightTableView : public QTableView {
  Q_OBJECT
  
public:
  /**
   * The default constrcutor passes _parent_ to QTableView.
   */
  explicit FlightTableView(QWidget* parent = nullptr);
  
  void setModel(QAbstractItemModel* model) override;
  
protected:
  void resizeEvent(QResizeEvent* event) override;
  
private:
  /**
   * Calculates size of each column in order to fit perfectly in the view.
   */
  void __resizeColumns();
  
};

#endif // FLIGHTTABLEVIEW_H
