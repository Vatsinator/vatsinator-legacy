/*
    controller.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "vatsinatorapplication.h"

#include "controller.h"
#include "defines.h"

QMap< int, QString> Controller::ratings;
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
    frequency(_data[4]),
    rating(_data[16].toInt()),
    atis(_data[35]),
    airport(NULL),
    isOK(true) {

  // clenup ATIS messahe
  if (atis[0] == '$') {
    auto index = atis.indexOf('^');
    atis.remove(0, index + 2);
  }

  atis.replace((QString)'^' + (char)167, "\n");

  __setMyIcaoAndFacility();
}

void
Controller::__setMyIcaoAndFacility() {
  QStringList sections = callsign.split('_');

  if (sections.back() == "CTR") {
    facility = CTR;
    airport = NULL;

    icao = sections.front();

    Fir* fir = FirDatabase::GetSingleton().findFirByIcao(icao);

    if (fir) {
      fir->addStaff(this);
      __produceDescription(fir);
    } else {
      // handle USA three-letters callsigns
      if (icao.length() == 3) {
        fir = FirDatabase::GetSingleton().findFirByIcao("K" + icao);

        if (fir) {
          fir->addStaff(this);
          __produceDescription(fir);
          return;
        }
      }

      for (const QString & alias: VatsimDataHandler::GetSingleton().getAliases().values(icao)) {
        fir = FirDatabase::GetSingleton().findFirByIcao(alias);

        if (fir) {
          fir->addStaff(this);
          __produceDescription(fir);
          return;
        }
      }

      Uir* uir = VatsimDataHandler::GetSingleton().findUIR(icao);

      if (uir) {
        uir->addStaff(this);
        __produceDescription(uir);
        return;
      }

      VatsinatorApplication::log("FIR could not be matched for: %s.", callsign.toStdString().c_str());

    }

    return;
  } else if (sections.back() == "FSS") {
    facility = FSS;
    airport = NULL;

    QString& icao = sections.front();

    Fir* fir = FirDatabase::GetSingleton().findFirByIcao(icao, true);

    if (fir) {
      fir->addStaff(this);
      __produceDescription(fir);
      return;
    }

    Uir* uir = VatsimDataHandler::GetSingleton().findUIR(icao);

    if (uir) {
      uir->addStaff(this);
      __produceDescription(uir);
      return;
    }

    fir = FirDatabase::GetSingleton().findFirByIcao(icao);

    if (fir) {
      fir->addStaff(this);
      __produceDescription(fir);
      return;
    }

    for (QString & alias: VatsimDataHandler::GetSingleton().getAliases().values(icao)) {
      fir = FirDatabase::GetSingleton().findFirByIcao(alias, true);

      if (fir) {
        fir->addStaff(this);
        __produceDescription(fir);
        return;
      }
    }

    VatsinatorApplication::log("FIR could not be matched for: %s.", callsign.toStdString().c_str());

    return;
  } else if (
    sections.back() == "APP" ||
    sections.back() == "DEP" ||
    sections.back() == "TWR" ||
    sections.back() == "GND" ||
    sections.back() == "DEL" ||
    sections.back() == "ATIS") {
    if (sections.back() == "APP")
      facility = APP;
    else if (sections.back() == "DEP")
      facility = DEP;
    else if (sections.back() == "TWR")
      facility = TWR;
    else if (sections.back() == "GND")
      facility = GND;
    else if (sections.back() == "DEL")
      facility = DEL;
    else if (sections.back() == "ATIS")
      facility = ATIS;

    const AirportRecord* apShot = AirportDatabase::GetSingleton().find(sections.front());

    if (apShot) {
      Airport* ap = VatsimDataHandler::GetSingleton().addActiveAirport(sections.front());
      ap->addStaff(this);
      airport = ap->getData();
      __produceDescription(airport);
      return;
    } else {
      if (sections.front().length() == 3) {
        // USA callsigns often are just three last letters of full airport ICAO code
        QString alias = "K" + sections.front();
        apShot = AirportDatabase::GetSingleton().find(alias);

        if (apShot) {
          Airport* ap = VatsimDataHandler::GetSingleton().addActiveAirport(alias);
          ap->addStaff(this);
          airport = ap->getData();
          __produceDescription(airport);
          return;
        }
      }

      for (QString & alias: VatsimDataHandler::GetSingleton().getAliases().values(sections.front())) {
        apShot = AirportDatabase::GetSingleton().find(alias);

        if (apShot) {
          Airport* ap = VatsimDataHandler::GetSingleton().addActiveAirport(alias);
          ap->addStaff(this);
          airport = ap->getData();
          __produceDescription(airport);
          return;
        }
      }

      VatsinatorApplication::log("Airport not found for %s.", callsign.toStdString().c_str());
    }

    return;

  }

  isOK = false;
}

void
Controller::__produceDescription(const Fir* _f) {
  Q_ASSERT(_f);
  description = _f->getName();
}

void
Controller::__produceDescription(const Uir* _u) {
  Q_ASSERT(_u);
  description = _u->name;
}

void
Controller::__produceDescription(const AirportRecord* _ap) {
  QString apName, fName; // airport name, facility name

  if (!_ap) {
    apName = "Unknown";
  } else {
    if (static_cast< QString >(_ap->name) == static_cast< QString >(_ap->city))
      apName = QString::fromUtf8(_ap->name);
    else
      apName =
        QString::fromUtf8(_ap->city) %
        "/" %
        QString::fromUtf8(_ap->name);
  }

  switch (facility) {
    case ATIS:
      fName = "ATIS";
      break;
    case DEL:
      fName = "Delivery";
      break;
    case GND:
      fName = "Ground";
      break;
    case TWR:
      fName = "Tower";
      break;
    case APP:
      fName = "Approach";
      break;
    case DEP:
      fName = "Departure";
      break;
    default:
      break;
  }

  description = apName % " " % fName;
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

