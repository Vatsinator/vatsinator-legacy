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
#include <QHBoxLayout>

#include "ui/widgets/abstractnotificationwidget.h"

class UrlButton;

class UpdateNotificationWidget : public AbstractNotificationWidget {
  
  /*
   * This is the red widget that opens up if UpdateChecker detects
   * outdated version of running Vatsinator.
   */
  
  Q_OBJECT
  
public:
  explicit UpdateNotificationWidget();
  
  virtual ~UpdateNotificationWidget();
  
  QBrush background() const;
  QColor foreground() const;
  
  UpdateNotificationWidget::Position position() const;
  
protected:
  void resizeEvent(QResizeEvent*);
  
private:
  // "Close this notification" button
  QPushButton* __closeButton;
  
  // "Visit Vatsinator homepage" button
  UrlButton* __visitButton;
  
  // Layout for above buttons
  QHBoxLayout* __layout;
  
};

#endif // UPDATENOTIFICATIONWIDGET_H
