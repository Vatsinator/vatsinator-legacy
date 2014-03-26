/*
 * flightitem.cpp
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

#include "db/airportdatabase.h"
#include "modules/modelmatcher.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/actions/metaraction.h"
#include "ui/actions/trackaction.h"
#include "ui/windows/flightdetailswindow.h"
#include "ui/windows/metarswindow.h"
#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "flightitem.h"
#include "defines.h"

FlightItem::FlightItem(const Pilot* _pilot, QObject* _parent) :
    QObject(_parent),
    __pilot(_pilot),
    __position(_pilot->position().longitude, _pilot->position().latitude),
    __model(ModelMatcher::getSingleton().matchMyModel(_pilot->aircraft()) ){
}

FlightItem::~FlightItem() {

}

void
FlightItem::drawModel() const {
  static const GLfloat modelRect[] = {
    -0.03, -0.03,
    -0.03,  0.03,
     0.03,  0.03,
     0.03, -0.03
  };
  
  glPushMatrix();
    glRotatef(static_cast<GLfloat>(data()->heading()), 0, 0, -1);
    glVertexPointer(2, GL_FLOAT, 0, modelRect);
    glBindTexture(GL_TEXTURE_2D, __model);
    glDrawArrays(GL_QUADS, 0, 4);
  glPopMatrix();
}

bool
FlightItem::needsDrawing() const {
  return !(data()->flightStatus() != Pilot::AIRBORNE || data()->isPrefiledOnly());
}

const QPointF &
FlightItem::position() const {
  return __position;
}

QString
FlightItem::tooltipText() const {
  QString callsign = data()->callsign();
  QString desc = QString("%1 (%2)").arg(data()->realName(), data()->aircraft());
  
  QString from;
  if (data()->route().origin.isEmpty()) {
    from = tr("(unknown)");
  } else {
    from = data()->route().origin;
    const AirportRecord* ap = VatsimDataHandler::getSingleton().activeAirports()[from]->data();
    if (ap)
      from += " " + QString::fromUtf8(ap->city);
  }
  
  QString to;
  if (data()->route().destination.isEmpty()) {
    to = tr("(unknown)");
  } else {
    to = data()->route().destination;
    const AirportRecord* ap = VatsimDataHandler::getSingleton().activeAirports()[to]->data();
    if (ap)
      to += " " + QString::fromUtf8(ap->city);
  }
  
  QString gs = tr("Ground speed: %1 kts").arg(QString::number(data()->groundSpeed()));
  QString alt = tr("Altitude: %1 ft").arg(QString::number(data()->altitude()));
  
  return QString("<p style='white-space:nowrap'><center>"
    % callsign % "<br />"
    % desc % "<br />"
    % from % " > " % to % "<br />"
    % gs % "<br />"
    % alt
    % "</center></p>");
}

QMenu *
FlightItem::menu(QWidget* _parent) const {
  QMenu* menu = new QMenu(data()->callsign(), _parent);
  
  ClientDetailsAction* showDetails = new ClientDetailsAction(data(), tr("Flight details"), _parent);
  connect(showDetails,                                  SIGNAL(triggered(const Client*)),
          FlightDetailsWindow::getSingletonPtr(),       SLOT(show(const Client*)));
  menu->addAction(showDetails);
  
  TrackAction* trackFlight = new TrackAction(data(), _parent);
  // TODO connect
  menu->addAction(trackFlight);
  
  menu->addSeparator();

  if (!data()->route().origin.isEmpty()) {
    MetarAction* ma = new MetarAction(data()->route().origin, _parent);
    connect(ma,                                 SIGNAL(triggered(QString)),
            MetarsWindow::getSingletonPtr(),    SLOT(show(QString)));
    menu->addAction(ma);
  }
  
  if (!data()->route().destination.isEmpty()) {
    MetarAction* ma = new MetarAction(data()->route().destination, _parent);
    connect(ma,                                 SIGNAL(triggered(QString)),
            MetarsWindow::getSingletonPtr(),    SLOT(show(QString)));
    menu->addAction(ma);
  }
  
  return menu;
}

void
FlightItem::showDetailsWindow() const {
  FlightDetailsWindow::getSingleton().show(data());
}
