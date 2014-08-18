/*
 * updatescheduler.cpp
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

#include "storage/settingsmanager.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "updatescheduler.h"

UpdateScheduler::UpdateScheduler(QObject* _parent): QObject(_parent) {
  __timer.setSingleShot(true);
  
  connect(&__timer,                     SIGNAL(timeout()),
          this,                         SIGNAL(timeToUpdate()));
  connect(qobject_cast<VatsimDataHandler*>(parent()), 
                                        SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(__setupTimer()));
}

void
UpdateScheduler::__setupTimer() {
  if (SM::get("network.auto_updater").toBool()) {
    int rate = vApp()->vatsimDataHandler()->reload() * 1000 * 60;
    __timer.setInterval(rate);
  } else {
    int rate = SM::get("network.refresh_rate").toInt() * 1000 * 60;
    __timer.setInterval(rate);
  }
  
  __timer.start();
}
