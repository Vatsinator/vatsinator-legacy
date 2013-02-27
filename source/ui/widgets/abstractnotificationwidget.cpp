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
  
  setGeometry(0, UserInterface::getSingleton().getMenuBar()->height(),
    UserInterface::getSingleton().width(), 30);
}

void
AbstractNotificationWidget::setText(const QString& _text) {
  __text = _text;
}

void
AbstractNotificationWidget::setBold(bool _b) {
  __isbold = _b;
}

void
AbstractNotificationWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  
  p.fillRect(rect(), background());
  
  QPen pen(foreground());
  p.setPen(pen);
  
  QFont font(QWidget::font());
  font.setPointSize(font.pointSize() + 1);
  if (__isbold)
    font.setWeight(QFont::DemiBold);
  p.setFont(font);
  
  p.translate(QPoint(10, (height() / 2) + 5));
  p.drawText(0, 0, __text);
}

