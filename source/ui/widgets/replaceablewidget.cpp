/*
    replaceablewidget.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include "replaceablewidget.h"
#include "defines.h"

ReplaceableWidget::ReplaceableWidget(QWidget* _parent) :
  QStackedWidget(_parent) {}

void
ReplaceableWidget::setWidgets(QWidget* _a, QWidget* _b) {
  __a = addWidget(_a);
  __b = addWidget(_b);
  setCurrentIndex(0);
}

void
ReplaceableWidget::toggle() {
  setCurrentIndex(currentIndex() == __a ? __b : __a);
}
