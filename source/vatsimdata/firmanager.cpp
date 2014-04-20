/*
 * firmanager.cpp
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

#include "db/firdatabase.h"
#include "vatsimdata/fir.h"

#include "firmanager.h"

FirManager::FirManager(QObject* _parent): QObject(_parent) {
  __init();
}

Fir*
FirManager::find(const QString& _icao) {
  auto result = qBinaryFind(__firs.begin(), __firs.end(), _icao,
    [](const Fir* a, const Fir* b) {
      return a->icao() < b->icao();
    }
  );
  
  if (result == __firs.end())
    return nullptr;
  else
    return *result;
}

const Fir* 
FirManager::find(const QString& _icao) const {
  auto result = qBinaryFind(__firs.begin(), __firs.end(), _icao,
    [](const Fir* a, const Fir* b) {
      return a->icao() < b->icao();
    }
  );
  
  if (result == __firs.end())
    return nullptr;
  else
    return *result;
}

void
FirManager::__init() {
  __firs.reserve(FirDatabase::getSingleton().firs().size());
  
  for (const Fir& f: FirDatabase::getSingleton().firs()) {
    __firs << new Fir(&f);
  }
}
