/*
 * clientdata.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include "clientdata.h"
#include "core/atc.h"
#include "core/pilot.h"
#include <QtCore>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Core {

class ClientDataData : public QSharedData {
public:
    ClientDataData() : pid(0), valid(false) {}
    
    QString callsign;
    quint32 pid;
    bool valid;
    QStringList line;
};

/**
 * Data line indexes.
 */
enum {
    Callsign = 0, Pid, RealName, ClientType, Frequency, Latitude, Longitude,
    Altitude, Groundspeed, PlannedAircraft, PlannedTascruise, PlannedDepAirport,
    PlannedAltitude, PlannedDestairport, Server, Protrevision, Rating,
    Transponder, FacilityType, VisualRange, PlannedRevision, PlannedFlightType,
    PlannedDepTime, ActualDepTime, PlannedHrsEnroute, PlannedMinsEnroute,
    PlannedHrsFuel, PlannedMinFuel, PlannedAltAirport, PlannedRemarks,
    PlannedRoute, PlannedDepAirportLat, PlannedDepAirportLon, PlannedDestAirportLat,
    PlannedDestAirportLon, AtisMessage, TimeLastAtisReceived, TimeLogon, Heading,
    QnhIhg, QnhMb
};

ClientData::ClientData() :
    d(new ClientDataData) {}

ClientData::ClientData(const ClientData& other) :
    d(other.d) {}

ClientData::~ClientData() {}

ClientData& ClientData::operator=(const ClientData& other)
{
    d = other.d;
    return *this;
}

QString ClientData::callsign() const
{
    return d->callsign;
}

void ClientData::setCallsign(const QString& callsign)
{
    d->callsign = callsign;
}

quint32 ClientData::pid() const
{
    return d->pid;
}

void ClientData::setPid(quint32 pid)
{
    d->pid = pid;
}

bool ClientData::valid() const
{
    return d->valid;
}

void ClientData::setValid(bool valid)
{
    d->valid = valid;
}

QStringList ClientData::line() const
{
    return d->line;
}

void ClientData::setLine(const QStringList& line)
{
    d->line = line;
}

Client* ClientData::createClient(ServerTracker* server)
{
    Client* client = nullptr;
    QString clientType = line().at(ClientType);
    if (clientType == QStringLiteral("PILOT")) {
        Pilot* p = new Pilot(pid(), server);
        updateClientImpl(p);
        updatePilotImpl(p);
        client = p;
    } else if (clientType == QStringLiteral("ATC")) {
        Atc* a = new Atc(pid(), server);
        updateClientImpl(a);
        updateAtcImpl(a);
        client = a;
    } else {
        return nullptr;
    }
    
    Q_ASSERT(client);
    return client;
}

void ClientData::updateClient(Client* client)
{
    updateClientImpl(client);
    
    if (Pilot* p = qobject_cast<Pilot*>(client)) {
        updatePilotImpl(p);
    } else if (Atc* a = qobject_cast<Atc*>(client)) {
        updateAtcImpl(a);
    }
}

ClientData ClientData::fromWhazzUpLine(const QString& line)
{
    QStringList data = line.split(':');
    
    ClientData client;
    if (data.length() == 42) { // line is valid
        client.setLine(data);
        client.setCallsign(data.at(0));
        bool ok;
        quint32 pid = data.at(1).toUInt(&ok);
        if (!ok) {
            client.setValid(false);
            return client;
        }
        
        if (data.at(ClientType) == QStringLiteral("PILOT") &&
                (data.at(Longitude).isEmpty() || data.at(Latitude).isEmpty())) {
            client.setValid(false);
            return client;
        }
        
        client.setPid(pid);
        client.setValid(true);
    } else {
        client.setValid(false);
    }
    
    return client;
}

void ClientData::updateClientImpl(Client* client)
{
    client->setCallsign(line().at(Callsign));
    client->setRealName(line().at(RealName));
    client->setServerName(line().at(Server));
    client->setOnlineFrom(QDateTime::fromString(line().at(TimeLogon), "yyyyMMddhhmmss"));
    bool ok1, ok2;
    qreal lon = line().at(Longitude).toDouble(&ok1);
    qreal lat = line().at(Latitude).toDouble(&ok2);
    
    if (ok1 && ok2)
        client->setPosition(LonLat(lon, lat));
}

void ClientData::updateAtcImpl(Atc* atc)
{
    atc->setFrequency(line().at(Frequency));
    atc->setRating(static_cast<Atc::Rating>(line().at(Rating).toInt()));
    atc->setAtis(line().at(AtisMessage));
    
    QStringList sections = atc->callsign().split('_');
    atc->icao = sections.front();
    
    QString facStr = sections.back();
    if (facStr == QStringLiteral("CTR"))
        atc->setFacility(Atc::Ctr);
    else if (facStr == QStringLiteral("FSS"))
        atc->setFacility(Atc::Fss);
    else if (facStr == QStringLiteral("APP"))
        atc->setFacility(Atc::App);
    else if (facStr == QStringLiteral("DEP"))
        atc->setFacility(Atc::Dep);
    else if (facStr == QStringLiteral("TWR"))
        atc->setFacility(Atc::Twr);
    else if (facStr == QStringLiteral("GND"))
        atc->setFacility(Atc::Gnd);
    else if (facStr == QStringLiteral("DEL"))
        atc->setFacility(Atc::Del);
    else if (facStr == QStringLiteral("ATIS"))
        atc->setFacility(Atc::Atis);
    else
        atc->setFacility(Atc::Obs);
}

void ClientData::updatePilotImpl(Pilot* pilot)
{
    pilot->setAltitude(line().at(Altitude).toInt());
    pilot->setGroundSpeed(line().at(Groundspeed).toInt());
    pilot->setSquawk(line().at(Transponder));
    pilot->setAircraft(line().at(PlannedAircraft));
    pilot->setTas(line().at(PlannedTascruise).toInt());
    pilot->setFlightRules(line().at(PlannedFlightType) == "I" ? Pilot::Ifr : Pilot::Vfr);
    pilot->setStd(QTime::fromString(line().at(PlannedDepTime), "hhmm"));
    pilot->setAtd(QTime::fromString(line().at(ActualDepTime), "hhmm"));
    pilot->setProgress(-1);
    pilot->setRemarks(line().at(PlannedRemarks));
    pilot->setHeading(line().at(Heading).toInt());
    pilot->setRoute(line().at(PlannedRoute));
    pilot->setCruiseAltitude(line().at(PlannedAltitude));
    pilot->setPlannedTas(line().at(PlannedTascruise).toInt());
    
    pilot->plannedDepartureAirport = line().at(PlannedDepAirport);
    pilot->plannedDestinationAirport = line().at(PlannedDestairport);
    
    if (pilot->std().isValid()) {
        QTime sta = QTime(pilot->std().hour() + line().at(PlannedHrsEnroute).toInt(),
                          pilot->std().minute() + line().at(PlannedMinsEnroute).toInt());
        pilot->setSta(sta);
    }
}

}} /* namespace Vatsinator::Core */
