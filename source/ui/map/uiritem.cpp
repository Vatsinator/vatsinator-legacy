/*
 * uiritem.cpp
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

#include "ui/map/mapscene.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/lonlat.h"
#include "vatsimdata/uir.h"

#include "uiritem.h"

UirItem::UirItem(const Uir* _uir, QObject* _parent) :
    QObject(_parent),
    __uir(_uir) {
  for (const Fir* f: data()->range()) {
    FirItem* fi = wui()->mainWindow()->mapWidget()->scene()->findItemForFir(f);
    if (fi)
      __firItems << fi;
  }
}

void
UirItem::showDetailsWindow() const {}

QMenu*
UirItem::menu(QWidget*) const {
  return nullptr;
}

QString
UirItem::tooltipText() const {
  return QString();
}

const LonLat&
UirItem::position() const {
  static LonLat ll;
  return ll;
}

bool
UirItem::needsDrawing() const {
  return !data()->isEmpty();
}
