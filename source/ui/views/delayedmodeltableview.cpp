/*
 * delayedmodeltableview.cpp
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

#include <QtGui>

#include "delayedmodeltableview.h"
#include "defines.h"

DelayedModelTableView::DelayedModelTableView(QWidget* _parent) :
    QTableView(_parent),
    __loadingText(tr("Loading...")) {}

DelayedModelTableView::DelayedModelTableView(QString _text, QWidget* _parent) :
    QTableView(_parent),
    __loadingText(std::move(_text)) {}

void
DelayedModelTableView::setLoadingText(const QString& _text) {
  __loadingText = _text;
}

void
DelayedModelTableView::paintEvent(QPaintEvent* _e) {
  if (model()) {
    QTableView::paintEvent(_e);
  } else {
    QPainter painter(viewport());
    painter.setPen(Qt::darkGray);
    painter.drawText(rect(), Qt::AlignCenter, __loadingText);
  }
}
