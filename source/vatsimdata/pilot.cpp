/*
    pilot.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include <QtGui>
#include <QtOpenGL>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "modules/modelmatcher.h"

#include "settings/settingsmanager.h"

#include "ui/mapwidget/mapwidget.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "pilot.h"
#include "defines.h"

/*
 * 0 callsign
 * 1 cid
 * 2 realname
 * 3 clienttype
 * 4 frequency
 * 5 latitude
 * 6 longitude
 * 7 altitude
 * 8 groundspeed
 * 9 planned_aircraft
 * 10 planned_tascruise
 * 11 planned_depairport
 * 12 planned_altitude
 * 13 planned_destairport
 * 14 server
 * 15 protrevision
 * 16 rating
 * 17 transponder
 * 18 facilitytype
 * 19 visualrange
 * 20 planned_revision
 * 21 planned_flighttype
 * 22 planned_deptime
 * 23 planned_actdeptime
 * 24 planned_hrsenroute
 * 25 planned_minenroute
 * 26 planned_hrsfuel
 * 27 planned_minfuel
 * 28 planned_altairport
 * 29 planned_remarks
 * 30 planned_route
 * 31 planned_depairport_lat
 * 32 planned_depairport_lon
 * 33 planned_destairport_lat
 * 34 planned_destairport_lon
 * 35 atis_message
 * 36 time_last_atis_received
 * 37 time_logon
 * 38 heading
 * 39 QNH_iHg
 * 40 QNH_Mb
 */
Pilot::Pilot(const QStringList& _data, bool _prefiled) :
    Client(_data),
    altitude(_data[7].toInt()),
    groundSpeed(_data[8].toInt()),
    squawk(_data[17]),
    aircraft(_data[9]),
    tas(_data[10].toInt()),
    flightRules(_data[21] == "I" ? IFR : VFR),
    remarks(_data[29]),
    heading(_data[38].toUInt()),
    route( {_data[11].toUpper(), _data[13].toUpper(), _data[30], _data[12].toUpper()}),
    prefiledOnly(_prefiled),
    __lineFrom(NULL),
    __lineTo(NULL),
    __callsignTip(0) {
  // vatsim sometimes skips the 0 on the beginning
  if (squawk.length() == 3)
    squawk = "0" + squawk;

  if (!route.origin.isEmpty()) {
    Airport* ap = VatsimDataHandler::GetSingleton().addActiveAirport(route.origin);
    ap->addOutbound(this);

    if (prefiledOnly && ap->getData()) {
      position.latitude = ap->getData()->latitude;
      position.longitude = ap->getData()->longitude;
    }

    if (ap->getFirs()[0])
      ap->getFirs()[0]->addFlight(this);

    if (ap->getFirs()[1])
      ap->getFirs()[1]->addFlight(this);
  }

  if (!route.destination.isEmpty()) {
    Airport* ap = VatsimDataHandler::GetSingleton().addActiveAirport(route.destination);
    ap->addInbound(this);

    if (ap->getFirs()[0])
      ap->getFirs()[0]->addFlight(this);

    if (ap->getFirs()[1])
      ap->getFirs()[1]->addFlight(this);
  }

  __setMyStatus();
  __generateLines();

  modelTexture = ModelMatcher::GetSingleton().matchMyModel(aircraft);
}

Pilot::~Pilot() {
  if (__callsignTip)
    MapWidget::deleteImage(__callsignTip);

  if (__lineFrom)
    delete [] __lineFrom;

  if (__lineTo)
    delete [] __lineTo;
}

void
Pilot::drawLineFrom() const {
  if (__lineFrom) {
    glColor4d(SettingsManager::GetSingleton().getOriginToPilotLineColor().redF(),
              SettingsManager::GetSingleton().getOriginToPilotLineColor().greenF(),
              SettingsManager::GetSingleton().getOriginToPilotLineColor().blueF(),
              1.0
             );
    glVertexPointer(2, GL_FLOAT, 0, __lineFrom);
    glDrawArrays(GL_LINES, 0, 2);
  }
}

void
Pilot::drawLineTo() const {
  if (__lineTo) {
    glColor4d(SettingsManager::GetSingleton().getPilotToDestinationLineColor().redF(),
              SettingsManager::GetSingleton().getPilotToDestinationLineColor().greenF(),
              SettingsManager::GetSingleton().getPilotToDestinationLineColor().blueF(),
              1.0
             );
    glVertexPointer(2, GL_FLOAT, 0, __lineTo);
    glLineStipple(3, 0xF0F0);
    glDrawArrays(GL_LINES, 0, 2);
    glLineStipple(1, 0xFFFF);
  }
}

void
Pilot::__setMyStatus() {
  if (!route.origin.isEmpty() && !route.destination.isEmpty()) {
    const AirportRecord* ap_origin =
      VatsimDataHandler::GetSingleton().getActiveAirports()[route.origin]->getData();
    const AirportRecord* ap_arrival =
      VatsimDataHandler::GetSingleton().getActiveAirports()[route.destination]->getData();

    if ((ap_origin == ap_arrival) && (ap_origin != NULL)) // traffic pattern?
      if (groundSpeed < 50) {
        flightStatus = DEPARTING;
        return;
      }

    if (ap_origin)
      if ((VatsimDataHandler::calcDistance(ap_origin->longitude, ap_origin->latitude,
                                           position.longitude, position.latitude) < PILOT_TO_AIRPORT) &&
      (groundSpeed < 50)) {
        flightStatus = DEPARTING;
        return;
      }

    if (ap_arrival)
      if ((VatsimDataHandler::calcDistance(ap_arrival->longitude, ap_arrival->latitude,
                                           position.longitude, position.latitude) < PILOT_TO_AIRPORT) &&
      (groundSpeed < 50)) {
        flightStatus = ARRIVED;
        return;
      }
  } else { // no flight plan
    if (groundSpeed > 50) {
      flightStatus = AIRBORNE;
      return;
    }

    const AirportRecord* closest = NULL;

    qreal distance = 0.0;

  for (const AirportRecord & ap: AirportDatabase::GetSingleton().getAirports()) {
      qreal temp = VatsimDataHandler::calcDistance(ap.longitude, ap.latitude,
                   position.longitude, position.latitude);

      if (((temp < distance) && closest) || !closest) {
        closest = &ap;
        distance = temp;
      }
    }

    if (closest) {
      if (distance > PILOT_TO_AIRPORT) {
        flightStatus = AIRBORNE;
        return;
      }

      route.origin = closest->icao;
      flightStatus = DEPARTING;
      return;
    }
  }

  flightStatus = AIRBORNE;
}

void
Pilot::__generateLines() {
  const Airport* ap = NULL;

  if (!route.origin.isEmpty())
    ap = VatsimDataHandler::GetSingleton().getActiveAirports()[route.origin];

  if (ap && ap->getData()) {
    __lineFrom = new GLfloat[4];

    double myLon = ap->getData()->longitude;
    double myLat = ap->getData()->latitude;

    if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
        VatsimDataHandler::calcDistance(myLon + 360, myLat, position.longitude, position.latitude))
      myLon += 360;
    else if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
             VatsimDataHandler::calcDistance(myLon - 360, myLat, position.longitude, position.latitude))
      myLon -= 360;

    __lineFrom[0] = myLon;
    __lineFrom[1] = myLat;
    __lineFrom[2] = position.longitude;
    __lineFrom[3] = position.latitude;
  }

  ap = NULL;

  if (!route.destination.isEmpty())
    ap = VatsimDataHandler::GetSingleton().getActiveAirports()[route.destination];

  if (ap && ap->getData()) {
    __lineTo = new GLfloat[4];

    double myLon = ap->getData()->longitude;
    double myLat = ap->getData()->latitude;

    if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
        VatsimDataHandler::calcDistance(myLon + 360, myLat, position.longitude, position.latitude))
      myLon += 360;
    else if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
             VatsimDataHandler::calcDistance(myLon - 360, myLat, position.longitude, position.latitude))
      myLon -= 360;

    __lineTo[0] = myLon;
    __lineTo[1] = myLat;
    __lineTo[2] = position.longitude;
    __lineTo[3] = position.latitude;
  }
}

GLuint
Pilot::__generateTip() const {
  QImage temp(MapWidget::GetSingleton().getPilotToolTipBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapWidget::GetSingleton().getPilotFont());
  painter.setPen(QColor(PILOTS_LABELS_FONT_COLOR));
  QRect rectangle(0, 1, 75, 14); // size of the tooltip.png
  painter.drawText(rectangle, Qt::AlignCenter, callsign);
  __callsignTip = MapWidget::loadImage(temp);
  return __callsignTip;
}

