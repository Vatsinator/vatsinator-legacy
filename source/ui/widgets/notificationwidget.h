/*
 * notificationwidget.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>

class UserInterface;

class NotificationWidget : public QWidget {
  
  Q_OBJECT
  
  friend class UserInterface;

public:
  
  enum Position {
    Top,
    Bottom
  };
  
  explicit NotificationWidget(Position = Top);
  
  virtual ~NotificationWidget();
  
protected:
  
  void setBoundingGeometry(const QRect&);
  
private:
  
  Position __position;

};

#endif // NOTIFICATIONWIDGET_H
