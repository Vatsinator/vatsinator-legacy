/*
 * mapscene.cpp
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

#include <QtCore>

#include "db/firdatabase.h"
#include "ui/map/firitem.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "mapscene.h"
#include "defines.h"

MapScene::MapScene(QObject* parent): QObject(parent) {
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(__updateData()));
  
  for (auto f: FirDatabase::getSingleton().firs())
    __firItems << new FirItem(&f);
}

void
MapScene::__updateData() {
  /**
   * TODO
   * 
   * This is wrong approach, as it removes all the objects and creates
   * new one. Instead, these objects should be only updated, old ones
   * removed and only these new ones added.
   */
  
  __staffedFirItems.clear();
  __unstaffedFirItems.clear();
  
  for (auto f: __firItems) {
    if (f->data()->isStaffed())
      __staffedFirItems << f;
    else
      __unstaffedFirItems << f;
  }
  
}
