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
#include "vatsimdata/lonlat.h"
#include "vatsimdata/uir.h"

#include "uiritem.h"

UirItem::UirItem(const Uir* uir, QObject* parent) :
    QObject(parent),
    __uir(uir) {
  
  MapScene* scene = qobject_cast<MapScene*>(parent);
  for (const Fir* f: data()->range()) {
    FirItem* fi = scene->findItemForFir(f);
    if (fi)
      __firItems << fi;
  }
}

bool
UirItem::isVisible() const {
  return !data()->isEmpty();
}

bool
UirItem::isLabelVisible() const {
  return false;
}

void
UirItem::showDetails() const {
  
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

void
UirItem::drawItem(QOpenGLShaderProgram*) const {
  
}

void UirItem::drawLabel(QOpenGLShaderProgram*) const {

}

void UirItem::drawFocused(QOpenGLShaderProgram*) const {

}
