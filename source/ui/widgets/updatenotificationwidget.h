/*
    updatenotificationwidget.h
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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


#ifndef UPDATENOTIFICATIONWIDGET_H
#define UPDATENOTIFICATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class UpdateNotificationWidget : public QWidget {
  
  /*
   * This is the red widget that opens up if UpdateChecker detects
   * outdated version of running Vatsinator.
   */
  
  Q_OBJECT
  
public:
  /**
   * UpdateNotificationWidget cannot run as a separate window,
   * prevent from calling non-parameter ctor.
   */
  explicit UpdateNotificationWidget(QWidget*);
  
  UpdateNotificationWidget() = delete;
  
  /**
   * Hides the widget.
   */
  virtual ~UpdateNotificationWidget();
  
protected:
  void resizeEvent(QResizeEvent*);
  
private:
  QLabel      __label;
  QPushButton __button;
  
private slots:
  void __openUrl(const QString&);
  
};

#endif // UPDATENOTIFICATIONWIDGET_H
