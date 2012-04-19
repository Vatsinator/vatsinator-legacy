/*
    Pilot.cpp
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

#include "../include/Pilot.h"

#include "../include/AirportsDatabase.h"
#include "../include/MapWidget.h"
#include "../include/SettingsWindow.h"
#include "../include/VatsimDataHandler.h"
#include "../include/defines.h"

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
Pilot::Pilot(const QStringList& _data) {
	callsign = _data[0];
	pid = _data[1].toUInt();
	realName = _data[2];
	position.latitude = _data[5].toDouble();
	position.longitude = _data[6].toDouble();
	altitude = _data[7].toInt();
	groundSpeed = _data[8].toInt();
	aircraft = _data[9];
	tas = _data[10].toInt();
	route.origin = _data[11];
	route.altitude = _data[12];
	route.destination = _data[13];
	server = _data[14];
	squawk = _data[17];
	flightRules = (_data[21] == "I") ? IFR : VFR;
	remarks = _data[29];
	route.route = _data[30];
	onlineFrom = QDateTime::fromString(_data[37], "yyyyMMddhhmmss");
	heading = _data[38].toUInt();
	
	if (!route.origin.isEmpty()) {
		AirportObject* ap = VatsimDataHandler::GetSingleton().addActiveAirport(route.origin);
		ap->addOutbound(this);
	}
	
	if (!route.destination.isEmpty()) {
		AirportObject* ap = VatsimDataHandler::GetSingleton().addActiveAirport(route.destination);
		ap->addInbound(this);
	}
	
	__setMyStatus();
	__generateTip();
}

Pilot::~Pilot() {
	MapWidget::deleteImage(callsignTip);
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
		for (const AirportRecord& ap: AirportsDatabase::GetSingleton().getAirports()) {
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
Pilot::__generateTip() {
	QImage temp(MapWidget::GetSingleton().getPilotToolTipBackground());
	QPainter painter(&temp);
	painter.setFont(MapWidget::GetSingleton().getPilotFont());
	painter.setPen(QColor(PILOTS_LABELS_FONT_COLOR));
	QRect rectangle(1, 1, 73, 15); // size of the tooltip.png
	painter.drawText(rectangle, Qt::AlignCenter, callsign);
	callsignTip = MapWidget::loadImage(temp);
}

