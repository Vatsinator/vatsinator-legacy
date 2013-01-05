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

#include "updatenotificationwidget.h"
#include "defines.h"

UpdateNotificationWidget::UpdateNotificationWidget(QWidget* _parent) :
    QWidget(_parent),
    __label(this),
    __button(this) {
  setVisible(false);
  setMinimumWidth(400);
  setFixedHeight(30);
  
  __label.setText(
    tr("There is a new version of Vatsinator available! Check it now at %1").arg(
      "<a href=\"" VATSINATOR_HOMEPAGE "\">" VATSINATOR_HOMEPAGE "</a>")
  );
  __label.setStyleSheet("QLabel { background: #ff7d7d; }");
  __label.setContentsMargins(10, 5, 10, 5);
  
  connect(&__label,    SIGNAL(linkActivated(const QString&)),
          this,        SLOT(__openUrl(const QString&)));
  
  __button.setText(tr("Close this notification"));
  
  connect(&__button,   SIGNAL(clicked()),
          this,        SLOT(hide()));
}

UpdateNotificationWidget::~UpdateNotificationWidget() {
  if (isVisible())
    hide();
}

void
UpdateNotificationWidget::resizeEvent(QResizeEvent* _event) {
  __label.resize(this->width(), this->height());
  
  QSize temp = __button.sizeHint();
  int margin = (this->height() - temp.height()) / 2;
  
  __button.setGeometry(this->width() - temp.width() - margin, margin,
                       temp.width(), temp.height());
  
  _event->accept();
}

void
UpdateNotificationWidget::__openUrl(const QString& _url) {
  QDesktopServices::openUrl(_url);
}
