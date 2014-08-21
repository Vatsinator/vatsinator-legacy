/*
    trackaction.cpp
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

#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"

#include "trackaction.h"

TrackAction::TrackAction(const Pilot* _pilot, QObject* _parent) :
    QAction(tr("Track this flight"), _parent),
    __current(_pilot) {
  
  setCheckable(true);
  if (__current == wui()->mainWindow()->mapWidget()->renderer()->scene()->trackedFlight())
    setChecked(true);

  connect(this, SIGNAL(triggered()), SLOT(__handleTriggered()));
  connect(wui()->mainWindow()->mapWidget()->renderer()->scene(),
                  SIGNAL(flightTracked(const Pilot*)),
          this,   SLOT(__updateChecked(const Pilot*)));
}

void
TrackAction::__handleTriggered() {
  wui()->mainWindow()->mapWidget()->renderer()->scene()->trackFlight(__current);
}

void
TrackAction::__updateChecked(const Pilot* _p) {
  setChecked(_p == __current);
}
