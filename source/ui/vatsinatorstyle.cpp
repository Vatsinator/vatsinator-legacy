/*
 * vatsinatorstyle.cpp
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "ui/windows/metarswindow.h"

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

void
VatsinatorStyle::polish(QWidget* widget) {
#ifdef Q_OS_MAC
  if (FlightDetailsWindow* w = dynamic_cast<FlightDetailsWindow*>(widget)) {
    w->layout()->setContentsMargins(12, 12, 12, 2);
  } else if (AtcDetailsWindow* w = dynamic_cast<AtcDetailsWindow*>(widget)) {
    w->layout()->setContentsMargins(12, 12, 12, 2);
  } else if (AirportDetailsWindow* w = dynamic_cast<AirportDetailsWindow*>(widget)) {
    w->layout()->setSpacing(4);
    w->layout()->setContentsMargins(12, 12, 12, 2);
  } else if (MetarsWindow* w = dynamic_cast<MetarsWindow*>(widget)) {
    w->layout()->setContentsMargins(0, 4, 0, 0);
    w->layout()->setSpacing(4);
    QLayout* l = w->findChild<QLayout*>("MetarControlLayout");
    Q_ASSERT(l);
    l->setContentsMargins(12, 0, 12, 0);
  }
  
#else
  Q_UNUSED(widget);
#endif
}
