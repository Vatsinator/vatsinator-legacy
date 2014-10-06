/*
    uir.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include "vatsimdata/fir.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/models/controllertablemodel.h"

#include "uir.h"

Uir::Uir(const QString& icao, QObject* parent) :
    QObject(parent),
    __icao(icao),
    __staff(new ControllerTableModel(this)) {}

void
Uir::addFir(Fir* fir) {
  __range.push_back(fir);
}

void
Uir::addStaff(const Controller* atc) {
  __staff->add(atc);
  connect(atc,           SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(atc,           SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
  
  for (Fir* f: range())
    f->addUirStaff(atc);
}

bool
Uir::isEmpty() const {
  return __staff->staff().isEmpty();
}

