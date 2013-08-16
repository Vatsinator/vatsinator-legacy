/*
 * notificationwidget.cpp
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

#include <QtGui>

#include "ui/userinterface.h"

#include "notificationwidget.h"
#include "defines.h"

NotificationWidget::NotificationWidget(Position _position) :
    QWidget(UserInterface::getSingletonPtr()->mapWidget()),
    __position(_position) {
  
  UserInterface::getSingleton().addNotifier(this);
  setVisible(true);
  
  setMinimumWidth(400);
  setFixedHeight(30);
}

NotificationWidget::~NotificationWidget() {
  UserInterface::getSingleton().removeNotifier(this);
}

void
NotificationWidget::setBoundingGeometry(const QRect& _rect) {
  QRect geometry = _rect;
  
  if (__position == Top) {
    geometry.setBottom(_rect.top() + 30);
  } else {
    geometry.setTop(_rect.bottom() - 30);
  }
  
  setGeometry(geometry);
  
  qDebug() << "New geometry: " << geometry;
}
