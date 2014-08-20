/*
    controller.cpp
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

#include <QtCore>

#include "db/airportdatabase.h"
#include "network/statspurveyor.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/uir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "controller.h"

QMap<int, QString> Controller::ratings;
bool Controller::__ratingsInitialized = Controller::__initRatings();

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
Controller::Controller(const QStringList& _data) :
    Client(_data),
    __frequency(_data[4]),
    __rating(_data[16].toInt()),
    __atis(_data[35]),
    __airport(nullptr),
    __isOK(true) {
  
  __cleanupAtis();
  __recognizeDetails();
}

void
Controller::update(const QStringList& _data) {
  Client::update(_data);
  __frequency = _data[4];
  __rating = _data[16].toInt();
  __atis = _data[35];
  __cleanupAtis();
  
  emit updated();
}

void Controller::__cleanupAtis() {
  // clenup ATIS message
  if (__atis[0] == '$') {
    int index = __atis.indexOf('^');
    __atis.remove(0, index + 2);
  }

  __atis.replace((QString)'^' + (char)167, "\n");
}

void
Controller::__recognizeDetails() {
  QStringList sections = callsign().split('_');

  if (sections.back() == "CTR") {
    __facility = Ctr;
    __airport = nullptr;

    __icao = sections.front();
    
    if (Fir* fir = vApp()->vatsimDataHandler()->findFir(__icao)) {
      fir->addStaff(this);
      __makeDescription(fir);
      return;
    }
    
    if (Uir* uir = vApp()->vatsimDataHandler()->findUir(__icao)) {
      uir->addStaff(this);
      __makeDescription(uir);
      return;
    }
      
    StatsPurveyor::getSingleton().reportNoAtc(callsign());
    VatsinatorApplication::log("FIR could not be matched for: %s.", qPrintable(callsign()));
    __isOK = false;
  } else if (sections.back() == "FSS") {
    __facility = Fss;
    __airport = nullptr;

    QString& icao = sections.front();

    Fir* fir = vApp()->vatsimDataHandler()->findFir(icao, true);
    if (!fir) {
      fir = vApp()->vatsimDataHandler()->findFir(icao, false);
    }

    if (fir) {
      fir->addStaff(this);
      __makeDescription(fir);
      return;
    }
    
    Uir* uir = vApp()->vatsimDataHandler()->findUir(icao);
    if (uir) {
      uir->addStaff(this);
      __makeDescription(uir);
      return;
    }
    
    StatsPurveyor::getSingleton().reportNoAtc(callsign());
    VatsinatorApplication::log("FIR could not be matched for: %s.", qPrintable(callsign()));
    __isOK = false;
  } else if (
    sections.back() == "APP" ||
    sections.back() == "DEP" ||
    sections.back() == "TWR" ||
    sections.back() == "GND" ||
    sections.back() == "DEL" ||
    sections.back() == "ATIS") {
    if (sections.back() == "APP")
      __facility = App;
    else if (sections.back() == "DEP")
      __facility = Dep;
    else if (sections.back() == "TWR")
      __facility = Twr;
    else if (sections.back() == "GND")
      __facility = Gnd;
    else if (sections.back() == "DEL")
      __facility = Del;
    else if (sections.back() == "ATIS")
      __facility = Atis;

    if (Airport* ap = vApp()->vatsimDataHandler()->findAirport(sections.front())) {
      ap->addStaff(this);
      __airport = ap;
      __makeDescription(__airport);
      return;
    }
    
    StatsPurveyor::getSingleton().reportNoAtc(callsign());
    VatsinatorApplication::log("Airport not found for %s.", qPrintable(callsign()));
  }
  
  __isOK = false;
}

void
Controller::__makeDescription(const Fir* _f) {
  Q_ASSERT(_f);
  QString aName = vApp()->vatsimDataHandler()->alternameName(_f->icao());
  if (aName.isEmpty())
    __description = _f->name();
  else
    __description = aName;
}

void
Controller::__makeDescription(const Uir* _u) {
  Q_ASSERT(_u);
  QString aName = vApp()->vatsimDataHandler()->alternameName(_u->icao());
  if (aName.isEmpty())
    __description = _u->name();
  else
    __description = aName;
}

void
Controller::__makeDescription(const Airport* _ap) {
  QString apName, fName; // airport name, facility name

  if (!_ap) {
    apName = "Unknown";
  } else {
    if (qstrcmp(_ap->data()->name, _ap->data()->city) == 0)
      apName = QString::fromUtf8(_ap->data()->name);
    else
      apName =
        QString::fromUtf8(_ap->data()->city) %
        "/" %
        QString::fromUtf8(_ap->data()->name);
  }

  switch (__facility) {
    case Atis:
      fName = "ATIS";
      break;
    case Del:
      fName = "Delivery";
      break;
    case Gnd:
      fName = "Ground";
      break;
    case Twr:
      fName = "Tower";
      break;
    case App:
      fName = "Approach";
      break;
    case Dep:
      fName = "Departure";
      break;
    default:
      break;
  }

  __description = apName % " " % fName;
}

bool
Controller::__initRatings() {

  // http://vateud.org/index.php?option=com_content&view=article&id=28&Itemid=133
  Controller::ratings[1] = "Observer";
  Controller::ratings[2] = "Student (S1)";
  Controller::ratings[3] = "Student 2 (S2)";
  Controller::ratings[4] = "Senior Student (S3)";
  Controller::ratings[5] = "Controller (C1)";
  Controller::ratings[7] = "Senior Controller (C3)";
  Controller::ratings[8] = "Instructor (I1)";
  Controller::ratings[10] = "Senior Instructor (I3)";
  Controller::ratings[11] = "Supervisor";
  Controller::ratings[12] = "Administrator";

  return true;
}

