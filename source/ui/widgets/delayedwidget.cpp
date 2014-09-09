/*
 * delayedwidget.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include <QtWidgets>

#include "delayedwidget.h"

DelayedWidget::DelayedWidget(QWidget* _parent, Qt::WindowFlags _f) :
    QWidget(_parent, _f),
    __status(Loading),
    __animation(nullptr) {}

void
DelayedWidget::setStatus(DelayedWidget::Status _status) {
  __status = _status;
  if (__animation && __animation->isValid()) {
    if (_status == Loading)
      __animation->start();
    else
      __animation->stop();
  }
}

void
DelayedWidget::setAnimation(QMovie* _animation) {
  __animation = _animation;
  
  if (_animation) {
    connect(_animation, &QMovie::updated, this,
            /* Many implementations of QWidget::update(), must be specific */
            (void (DelayedWidget::*)())&DelayedWidget::update,
            Qt::DirectConnection);
    if (__status == Loading)
      __animation->start();
  }
}

void
DelayedWidget::paintEvent(QPaintEvent* _event) {
  if (status() == Loading && __animation) {
    QPainter p(this);
    QPixmap pixmap = __animation->currentPixmap();
    
    QRect rect(QPoint(0, 0), pixmap.size());
    rect.moveCenter(_event->rect().center());
    
    p.drawPixmap(rect, pixmap);
  }
}
