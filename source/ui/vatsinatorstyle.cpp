/*
 * vatsinatorstyle.cpp
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

#include "vatsinatorstyle.h"

QFont
VatsinatorStyle::boldFont() {
  QFont font;
  font.setBold(true);
  return font;
}

QFont
VatsinatorStyle::h1Font() {
  QFont font;
  int size = font.pointSize() + 4;
  font.setPointSize(size);
  font.setBold(true);
  return font;
}

QFont
VatsinatorStyle::h2Font() {
  QFont font;
  int size = font.pointSize() + 2;
  font.setPointSize(size);
  font.setBold(true);
  return font;
}

QFont
VatsinatorStyle::smallFont() {
  QFont font;
  int size = font.pointSize() - 2;
  font.setPointSize(size);
  return font;
}

#ifdef Q_OS_MAC
void
VatsinatorStyle::polish(QWidget* widget) {
  if (!qobject_cast<QMenu*>(widget) && widget->testAttribute(Qt::WA_MacNormalSize))
    widget->setAttribute(Qt::WA_MacMiniSize);
}
#endif
