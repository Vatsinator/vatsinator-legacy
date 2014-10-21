/*
    replaceablewidget.h
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


#ifndef REPLACEABLEWIDGET_H
#define REPLACEABLEWIDGET_H

#include <initializer_list>
#include <QStackedWidget>

/**
 * The ReplaceableWidget is used prior to display label that
 * changes to progress bar, which is visible at the
 * bottom-left corner of the main window.
 */
class ReplaceableWidget : public QStackedWidget {
  Q_OBJECT
  
public:
  /**
   * Passes the _parent_ to QStackedWidget's constructor.
   */
  ReplaceableWidget(QWidget* parent = nullptr);
  
  /**
   * Adds given widgets.
   */
  void addWidgets(const std::initializer_list<QWidget*>& widgets);
  
  /**
   * Changes current widget to the next.
   */
  void next();
  
};

#endif // REPLACEABLEWIDGET_H
