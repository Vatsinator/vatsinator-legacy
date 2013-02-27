/*
    updatenotificationwidget.cpp
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

#include "ui/buttons/urlbutton.h"

#include "updatenotificationwidget.h"
#include "defines.h"

UpdateNotificationWidget::UpdateNotificationWidget() :
    AbstractNotificationWidget(),
    __closeButton(new QPushButton(tr("&Close this notification"), this)),
    __visitButton(new UrlButton(tr("&Download new version now!"), VATSINATOR_HOMEPAGE, this)),
    __layout(new QHBoxLayout(this)) {
  setVisible(false);
  setBold(true);
  
  setText(tr("There is a new version of Vatsinator available!"));
  
  __visitButton->setDefault(true);
  
  __layout->setContentsMargins(5, 2, 5, 2);
  
  __layout->addWidget(__visitButton, 1);
  __layout->addWidget(__closeButton);
  
  connect(__closeButton,   SIGNAL(clicked()),
          this,            SLOT(hide()));
}

UpdateNotificationWidget::~UpdateNotificationWidget() {
  delete __layout;
}

QBrush
UpdateNotificationWidget::background() const {
  return QBrush(QColor("#ff7d7d"));
}

QColor
UpdateNotificationWidget::foreground() const {
  return QColor("#000000");
}

void
UpdateNotificationWidget::resizeEvent(QResizeEvent* _event) {
  QSize temp = __closeButton->sizeHint();
  temp.rwidth() *= 3;
  temp.rheight() = this->height();
  
  __layout->setGeometry(QRect(
    QPoint(this->width() - temp.width(), 0),
    temp
  ));
  
  _event->accept();
}

