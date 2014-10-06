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

#include <QtWidgets>

#include "delayedmodeltableview.h"

DelayedModelTableView::DelayedModelTableView(QWidget* parent) :
    QTableView(parent),
    __loadingText(tr("Loading...")),
    __errorText(tr("Error occured")),
    __errorOnNoData(true) {}

void
DelayedModelTableView::setLoadingText(const QString& text) {
  __loadingText = text;
}

void
DelayedModelTableView::setErrorText(const QString& text) {
  __errorText = text;
}

void
DelayedModelTableView::setErrorOnNoData(bool value) {
  __errorOnNoData = value;
}

void
DelayedModelTableView::paintEvent(QPaintEvent* event) {
  if (model()) {
    if ((model()->rowCount() == 0 || model()->columnCount() == 0) && __errorOnNoData) {
      QPainter painter(viewport());
      painter.setPen(Qt::darkGray);
      painter.drawText(rect(), Qt::AlignCenter, __errorText);
    } else {
      QTableView::paintEvent(event);
    }
  } else {
    QPainter painter(viewport());
    painter.setPen(Qt::darkGray);
    painter.drawText(rect(), Qt::AlignCenter, __loadingText);
  }
}
