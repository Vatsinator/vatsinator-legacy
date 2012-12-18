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

#include "ui/widgets/mapwidget.h"

#include "vatsimdata/airport/activeairport.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

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
    pressure({_data[39], _data[40]}),
    route({_data[11].toUpper(), _data[13].toUpper(), _data[30], _data[12].toUpper()}),
    prefiledOnly(_prefiled),
    __lineFrom(0),
    __lineTo(0),
    __callsignTip(0) {
  // vatsim sometimes skips the 0 on the beginning
  if (squawk.length() == 3)
    squawk.prepend("0");

  if (!route.origin.isEmpty()) {
    ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(route.origin);
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
    ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(route.destination);
    ap->addInbound(this);

    if (ap->getFirs()[0])
      ap->getFirs()[0]->addFlight(this);

    if (ap->getFirs()[1])
      ap->getFirs()[1]->addFlight(this);
  }

  __setMyStatus();
  __generateLines();

  modelTexture = ModelMatcher::getSingleton().matchMyModel(aircraft);
}

Pilot::~Pilot() {
  if (__callsignTip)
    MapWidget::deleteImage(__callsignTip);
}

void
Pilot::drawLineFrom() const {
  if (!__lineFrom.empty()) {
    glColor4d(SettingsManager::getSingleton().getOriginToPilotLineColor().redF(),
              SettingsManager::getSingleton().getOriginToPilotLineColor().greenF(),
              SettingsManager::getSingleton().getOriginToPilotLineColor().blueF(),
              1.0
             );
    glVertexPointer(2, GL_FLOAT, 0, &__lineFrom[0]);
    glDrawArrays(GL_LINE_STRIP, 0, __lineFrom.size() / 2);
  }
}

void
Pilot::drawLineTo() const {
  if (!__lineTo.empty()) {
    glColor4d(SettingsManager::getSingleton().getPilotToDestinationLineColor().redF(),
              SettingsManager::getSingleton().getPilotToDestinationLineColor().greenF(),
              SettingsManager::getSingleton().getPilotToDestinationLineColor().blueF(),
              1.0
             );
    glVertexPointer(2, GL_FLOAT, 0, &__lineTo[0]);
    glLineStipple(3, 0xF0F0);
    glDrawArrays(GL_LINE_STRIP, 0, __lineTo.size() / 2);
    glLineStipple(1, 0xFFFF);
  }
}

void
Pilot::__setMyStatus() {
  if (!route.origin.isEmpty() && !route.destination.isEmpty()) {
    const AirportRecord* ap_origin =
      VatsimDataHandler::getSingleton().getActiveAirports()[route.origin]->getData();
    const AirportRecord* ap_arrival =
      VatsimDataHandler::getSingleton().getActiveAirports()[route.destination]->getData();

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

    for (const AirportRecord & ap: AirportDatabase::getSingleton().getAirports()) {
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
      ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(route.origin);
      ap->addOutbound(this);
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
    ap = VatsimDataHandler::getSingleton().getActiveAirports()[route.origin];

  if (ap && ap->getData()) {
    double myLon = ap->getData()->longitude;
    double myLat = ap->getData()->latitude;

    if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
        VatsimDataHandler::calcDistance(myLon + 360, myLat, position.longitude, position.latitude))
      myLon += 360;
    else if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
             VatsimDataHandler::calcDistance(myLon - 360, myLat, position.longitude, position.latitude))
      myLon -= 360;

    __lineFrom << myLon
               << myLat;
  }
  
  __lineTo << position.longitude
           << position.latitude;
  
  __parseRoute();
  
  __lineFrom << position.longitude
             << position.latitude;

  ap = NULL;

  if (!route.destination.isEmpty())
    ap = VatsimDataHandler::getSingleton().getActiveAirports()[route.destination];

  if (ap && ap->getData()) {
    double myLon = ap->getData()->longitude;
    double myLat = ap->getData()->latitude;

    if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
        VatsimDataHandler::calcDistance(myLon + 360, myLat, position.longitude, position.latitude))
      myLon += 360;
    else if (VatsimDataHandler::calcDistance(myLon, myLat, position.longitude, position.latitude) >
             VatsimDataHandler::calcDistance(myLon - 360, myLat, position.longitude, position.latitude))
      myLon -= 360;

    __lineTo << myLon
             << myLat;
  }
}

GLuint
Pilot::__generateTip() const {
  QImage temp(MapWidget::getSingleton().getPilotToolTipBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapWidget::getSingleton().getPilotFont());
  painter.setPen(QColor(PILOTS_LABELS_FONT_COLOR));
  QRect rectangle(28, 10, 73, 13); // size of the tooltip.png
  painter.drawText(rectangle, Qt::AlignCenter, callsign);
  __callsignTip = MapWidget::loadImage(temp);
  return __callsignTip;
}

void
Pilot::__parseRoute() {
  /*
   * Okay, this function needs some comment.
   * Pilots generally fill their NATs in three different ways.
   * 1) YQX KOBEV 50N50W 51N40W 52N30W 52N20W LIMRI XETBO
   * 2) VIXUN LOGSU 4950N 5140N 5130N 5120N DINIM
   * 3) 
   * TODO: Include NATs entry/exit points.
   */
  
  int pos = 0;
  QVector< float >* curList = &__lineFrom;
  
  /* 1 */
  static QRegExp expNo1(" ([0-9]{2}[NS][0-9]{2,3}[SW])", Qt::CaseSensitive, QRegExp::RegExp2);
  while ((pos = expNo1.indexIn(this->route.route, pos)) != -1) {
    pos += expNo1.matchedLength();
  }
  
  pos = 0;
  
  
  static QRegExp expNo2(" ([0-9]{4}[NS])", Qt::CaseSensitive, QRegExp::RegExp2);
  while ((pos = expNo2.indexIn(this->route.route, pos)) != -1) {
    
    QString cap = expNo2.cap(1);
    
    float ns = cap.left(2).toFloat();
    if (cap.endsWith('S'))
      ns = -ns;
    
    float west = 0 - cap.mid(2, 2).toFloat();
    
    if (curList == &__lineFrom && !__lineFrom.isEmpty() && (
        (__lineFrom[__lineFrom.size() - 2] < position.longitude && west > position.longitude) ||
        (__lineFrom[__lineFrom.size() - 2] > position.longitude && west < position.longitude)))
      curList = &__lineTo;
    
    *curList << west << ns;
    
    pos += expNo2.matchedLength();
  }
  
}

