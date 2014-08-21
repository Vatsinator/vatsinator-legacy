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

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

#include "delayedmodeltableview.h"

DelayedModelTableView::DelayedModelTableView(QWidget* _parent) :
    QTableView(_parent),
    __loadingText(tr("Loading...")),
    __errorText(tr("Error occured")),
    __errorOnNoData(true) {}

void
DelayedModelTableView::setLoadingText(const QString& _text) {
  __loadingText = _text;
}

void
DelayedModelTableView::setErrorText(const QString& _text) {
  __errorText = _text;
}

void
DelayedModelTableView::setErrorOnNoData(bool _eond) {
  __errorOnNoData = _eond;
}

void
DelayedModelTableView::paintEvent(QPaintEvent* _e) {
  if (model()) {
    if ((model()->rowCount() == 0 || model()->columnCount() == 0) && __errorOnNoData) {
      QPainter painter(viewport());
      painter.setPen(Qt::darkGray);
      painter.drawText(rect(), Qt::AlignCenter, __errorText);
    } else {
      QTableView::paintEvent(_e);
    }
  } else {
    QPainter painter(viewport());
    painter.setPen(Qt::darkGray);
    painter.drawText(rect(), Qt::AlignCenter, __loadingText);
  }
}
