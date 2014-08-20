/*
    toggleinboundoutboundlinesaction.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include "db/airportdatabase.h"
#include "vatsimdata/airport.h"

#include "toggleinboundoutboundlinesaction.h"

ToggleInboundOutboundLinesAction::ToggleInboundOutboundLinesAction(
  const Airport* _ap, QObject* _parent) :
    QAction(tr("Toggle inbound/outbound lines"), _parent),
    __current(_ap) {
  setCheckable(true);
//   if (AirportTracker::getSingleton().tracked().contains(QString(_ap->data()->icao)))
//     setChecked(true);
  
  connect(this, SIGNAL(triggered()), this, SLOT(__handleTriggered()));
}

void
ToggleInboundOutboundLinesAction::__handleTriggered() {
  emit triggered(__current);
}

