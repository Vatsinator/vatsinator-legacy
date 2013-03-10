/*
    abstractnotificationwidget.cpp
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

#include <QtGui>

#include "ui/userinterface.h"

#include "abstractnotificationwidget.h"
#include "defines.h"

AbstractNotificationWidget::AbstractNotificationWidget() :
    QWidget(UserInterface::getSingletonPtr()) {
  setMinimumWidth(400);
  setFixedHeight(30);
}

void
AbstractNotificationWidget::setBoundingGeometry(const QRect& _rect) {
  if (position() == Top)
    setGeometry(QRect(_rect.x(), _rect.y(),
                      _rect.width(), this->sizeHint().height()));
  else
    setGeometry(QRect(_rect.x(), _rect.y() + _rect.height() - this->sizeHint().height(),
                      _rect.width(), this->sizeHint().height()));
}

void
AbstractNotificationWidget::setBoundingGeometry(int x, int y, int w, int h) {
  if (position() == Top)
    setGeometry(QRect(x, y,
                      w, this->sizeHint().height()));
  else
    setGeometry(QRect(x, y + h - this->sizeHint().height(),
                      w, this->sizeHint().height()));
}


