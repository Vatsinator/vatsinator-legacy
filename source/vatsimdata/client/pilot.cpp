/*
    pilot.cpp
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

#include <QtGui>
#include <QtOpenGL>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "glutils/glresourcemanager.h"

#include "modules/modelmatcher.h"

#include "storage/settingsmanager.h"

#include "ui/pages/colorspage.h"
#include "ui/widgets/mapwidget.h"

#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "pilot.h"
#include "defines.h"

// how far from the airport the pilot must be to be recognized as "departing"
// or "arrived"
static const qreal PILOT_TO_AIRPORT = 0.1;

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
    __altitude(_data[7].toInt()),
    __groundSpeed(_data[8].toInt()),
    __squawk(_data[17]),
    __aircraft(_data[9]),
    __tas(_data[10].toInt()),
    __flightRules(_data[21] == "I" ? IFR : VFR),
    __remarks(_data[29]),
    __heading(_data[38].toUInt()),
    __pressure({_data[39], _data[40]}),
    __route({_data[11].toUpper(), _data[13].toUpper(), _data[30], _data[12].toUpper()}),
    __prefiledOnly(_prefiled),
    __lineFrom(0),
    __lineTo(0),
    __linesGenerated(false),
    __callsignTip(0) {
  // vatsim sometimes skips the 0 on the beginning
  if (__squawk.length() == 3)
    __squawk.prepend("0");

  __updateAirports();
  __setMyStatus();
//   __generateLines();

  __modelTexture = ModelMatcher::getSingleton().matchMyModel(__aircraft);
}

Pilot::~Pilot() {
  if (__callsignTip)
    GlResourceManager::deleteImage(__callsignTip);
}

void
Pilot::drawLineFrom() const {
  if (!__linesGenerated)
    __generateLines();
  
  if (!__lineFrom.empty()) {
    QColor otp = SM::get("colors.origin_to_pilot_line").value<QColor>();
    glColor4f(otp.redF(),
              otp.greenF(),
              otp.blueF(),
              1.0
             );
    
    glVertexPointer(2, GL_FLOAT, 0, &__lineFrom[0]);
    glDrawArrays(GL_LINE_STRIP, 0, __lineFrom.size() / 2);
  }
}

void
Pilot::drawLineTo() const {
  if (!__linesGenerated)
    __generateLines();
  
  if (!__lineTo.empty()) {
    QColor ptd = SM::get("colors.pilot_to_destination_line").value<QColor>();
    glColor4f(ptd.redF(),
              ptd.greenF(),
              ptd.blueF(),
              1.0
             );
    glVertexPointer(2, GL_FLOAT, 0, &__lineTo[0]);
    glLineStipple(3, 0xF0F0);
    glDrawArrays(GL_LINE_STRIP, 0, __lineTo.size() / 2);
    glLineStipple(1, 0xFFFF);
  }
}

void Pilot::__updateAirports() {
  if (!__route.origin.isEmpty()) {
    ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(__route.origin);
    ap->addOutbound(this);

    if (__prefiledOnly && ap->data()) {
      __position.latitude = ap->data()->latitude;
      __position.longitude = ap->data()->longitude;
    }

    if (ap->firs()[0])
      ap->firs()[0]->addFlight(this);

    if (ap->firs()[1])
      ap->firs()[1]->addFlight(this);
  }

  if (!__route.destination.isEmpty()) {
    ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(__route.destination);
    ap->addInbound(this);

    if (ap->firs()[0])
      ap->firs()[0]->addFlight(this);

    if (ap->firs()[1])
      ap->firs()[1]->addFlight(this);
  }
}

void
Pilot::__setMyStatus() {
  if (!__route.origin.isEmpty() && !__route.destination.isEmpty()) {
    const AirportRecord* ap_origin =
      VatsimDataHandler::getSingleton().activeAirports()[__route.origin]->data();
    const AirportRecord* ap_arrival =
      VatsimDataHandler::getSingleton().activeAirports()[__route.destination]->data();

    if ((ap_origin == ap_arrival) && (ap_origin != NULL)) // traffic pattern?
      if (__groundSpeed < 50) {
        __flightStatus = DEPARTING;
        return;
      }

    if (ap_origin)
      if ((VatsimDataHandler::distance(ap_origin->longitude, ap_origin->latitude,
                                           __position.longitude, __position.latitude) < PILOT_TO_AIRPORT) &&
      (__groundSpeed < 50)) {
        __flightStatus = DEPARTING;
        return;
      }

    if (ap_arrival)
      if ((VatsimDataHandler::distance(ap_arrival->longitude, ap_arrival->latitude,
                                           __position.longitude, __position.latitude) < PILOT_TO_AIRPORT) &&
      (__groundSpeed < 50)) {
        __flightStatus = ARRIVED;
        return;
      }
  } else { // no flight plan
    if (__groundSpeed > 50) {
      __flightStatus = AIRBORNE;
      return;
    }

    const AirportRecord* closest = NULL;
    qreal distance = 0.0;

    for (const AirportRecord & ap: AirportDatabase::getSingleton().airports()) {
      qreal temp = VatsimDataHandler::distance(ap.longitude, ap.latitude,
                   __position.longitude, __position.latitude);

      if (((temp < distance) && closest) || !closest) {
        closest = &ap;
        distance = temp;
      }
    }

    if (closest) {
      if (distance > PILOT_TO_AIRPORT) {
        __flightStatus = AIRBORNE;
        return;
      }

      __route.origin = closest->icao;
      ActiveAirport* ap = VatsimDataHandler::getSingleton().addActiveAirport(__route.origin);
      ap->addOutbound(this);
      __flightStatus = DEPARTING;
      return;
    }
  }

  __flightStatus = AIRBORNE;
}

void
Pilot::__generateLines() const {
  const Airport* ap = NULL;

  if (!__route.origin.isEmpty())
    ap = VatsimDataHandler::getSingleton().activeAirports()[__route.origin];

  if (ap && ap->data()) {
    double myLon = ap->data()->longitude;
    double myLat = ap->data()->latitude;

    if (VatsimDataHandler::distance(myLon, myLat, __position.longitude, __position.latitude) >
        VatsimDataHandler::distance(myLon + 360, myLat, __position.longitude, __position.latitude))
      myLon += 360;
    else if (VatsimDataHandler::distance(myLon, myLat, __position.longitude, __position.latitude) >
             VatsimDataHandler::distance(myLon - 360, myLat, __position.longitude, __position.latitude))
      myLon -= 360;

    __lineFrom << myLon
               << myLat;
  }
  
  __lineTo << __position.longitude
           << __position.latitude;
  
  __parseRoute();
  
  __lineFrom << __position.longitude
             << __position.latitude;

  ap = NULL;

  if (!__route.destination.isEmpty())
    ap = VatsimDataHandler::getSingleton().activeAirports()[__route.destination];

  if (ap && ap->data()) {
    double myLon = ap->data()->longitude;
    double myLat = ap->data()->latitude;

    if (VatsimDataHandler::distance(myLon, myLat, __position.longitude, __position.latitude) >
        VatsimDataHandler::distance(myLon + 360, myLat, __position.longitude, __position.latitude))
      myLon += 360;
    else if (VatsimDataHandler::distance(myLon, myLat, __position.longitude, __position.latitude) >
             VatsimDataHandler::distance(myLon - 360, myLat, __position.longitude, __position.latitude))
      myLon -= 360;

    __lineTo << myLon
             << myLat;
  }
  
  __linesGenerated = true;
}

GLuint
Pilot::__generateTip() const {
  QImage temp(MapWidget::getSingleton().pilotToolTipBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapWidget::getSingleton().pilotFont());
  painter.setPen(MapWidget::getSingleton().pilotPen());
  QRect rectangle(28, 10, 73, 13); // size of the tooltip.png
  painter.drawText(rectangle, Qt::AlignCenter, __callsign);
  __callsignTip = GlResourceManager::loadImage(temp);
  return __callsignTip;
}

void
Pilot::__parseRoute() const {
  /*
   * Okay, this function needs some comment.
   * Pilots generally fill their NATs in several different ways, but
   * NAT[A-Z] is the worst for us.
   * TODO: Include NATs entry/exit points.
   */
  
  int pos = 0;
  QVector<float>* curList = &__lineFrom;
  bool natFound = false;
  
  /* 1) 5000N 05000W 5100N 04000W 5100N 03000W 5100N 02000W */
  static QRegExp expNo1(" ([0-9]{4}[NS]) ([0-9]{5}[EW])", Qt::CaseSensitive, QRegExp::RegExp2);
  while ((pos = expNo1.indexIn(this->__route.route, pos)) != -1) {
    QString cap1 = expNo1.cap(1);
    
    float ns = cap1.left(2).toFloat();
    if (cap1.endsWith('S'))
      ns = -ns;
    
    QString cap2 = expNo1.cap(2);
    
    float we = cap2.left(3).toFloat();
    if (cap2.endsWith('W'))
      we = -we;
    
    if (curList == &__lineFrom && !__lineFrom.isEmpty() && (
        (__lineFrom[__lineFrom.size() - 2] < __position.longitude && we > __position.longitude) ||
        (__lineFrom[__lineFrom.size() - 2] > __position.longitude && we < __position.longitude)))
      curList = &__lineTo;
    
    *curList << we << ns;
    
    pos += expNo1.matchedLength();
    
    natFound = true;
  }
  
  if (natFound)
    return;
  
  pos = 0;
  
  /* 2) YQX KOBEV 50N50W 51N40W 52N30W 52N20W LIMRI XETBO */
  static QRegExp expNo2(" ([0-9]{2}[NS])([0-9]{2,3}[EW])", Qt::CaseSensitive, QRegExp::RegExp2);
  while ((pos = expNo2.indexIn(this->__route.route, pos)) != -1) {
    QString cap1 = expNo2.cap(1);
    
    float ns = cap1.left(2).toFloat();
    if (cap1.endsWith('S'))
      ns = -ns;
    
    QString cap2 = expNo2.cap(2);
    
    float we;
    if (cap2.length() > 3)
      we = cap2.left(3).toFloat();
    else
      we = cap2.left(2).toFloat();
    
    if (cap2.endsWith('W'))
      we = -we;
    
    if (curList == &__lineFrom && !__lineFrom.isEmpty() && (
        (__lineFrom[__lineFrom.size() - 2] < __position.longitude && we > __position.longitude) ||
        (__lineFrom[__lineFrom.size() - 2] > __position.longitude && we < __position.longitude)))
      curList = &__lineTo;
    
    *curList << we << ns;
    
    pos += expNo2.matchedLength();
    
    natFound = true;
  }
  
  if (natFound)
    return;
  
  pos = 0;
  
  
  /* 3) VIXUN LOGSU 4950N 5140N 5130N 5120N DINIM */
  static QRegExp expNo3(" ([0-9]{4}[NS]) ", Qt::CaseSensitive, QRegExp::RegExp2);
  while ((pos = expNo3.indexIn(this->__route.route, pos)) != -1) {
    QString cap = expNo3.cap(1);
    
    float ns = cap.left(2).toFloat();
    if (cap.endsWith('S'))
      ns = -ns;
    
    float west = 0 - cap.mid(2, 2).toFloat();
    
    if (curList == &__lineFrom && !__lineFrom.isEmpty() && (
        (__lineFrom[__lineFrom.size() - 2] < __position.longitude && west > __position.longitude) ||
        (__lineFrom[__lineFrom.size() - 2] > __position.longitude && west < __position.longitude)))
      curList = &__lineTo;
    
    *curList << west << ns;
    
    pos += expNo3.matchedLength();
    
    natFound = true;
  }
  
  if (natFound)
    return;
  
  pos = 0;
  
  /* 4) MALOT 54/20 54/30 54/40 53/50 HECKK */
  static QRegExp expNo4(" ([0-9]{2}/[0-9]{2})", Qt::CaseSensitive, QRegExp::RegExp2);
  while ((pos = expNo4.indexIn(this->__route.route, pos)) != -1) {
    QString cap = expNo4.cap(1);
    
    float north = cap.left(2).toFloat();
    float west = 0 - cap.right(2).toFloat();
    
    if (curList == &__lineFrom && !__lineFrom.isEmpty() && (
        (__lineFrom[__lineFrom.size() - 2] < __position.longitude && west > __position.longitude) ||
        (__lineFrom[__lineFrom.size() - 2] > __position.longitude && west < __position.longitude)))
      curList = &__lineTo;
    
    *curList << west << north;
    
    pos += expNo4.matchedLength();
    
    natFound = true;
  }
  
  if (natFound)
    return;
  
  pos = 0;
}

