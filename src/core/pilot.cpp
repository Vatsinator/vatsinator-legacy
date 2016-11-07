/*
 * pilot.cpp
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#include "pilot.h"
#include "geo.h"
#include "natparser.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class Pilot::RouteParserTask : public QRunnable {
public:
    RouteParserTask(Pilot* pilot) : m_pilot(pilot) {}
    void run() override;

private:
    Pilot* const m_pilot;
};


Pilot::Pilot(quint32 pid, ServerTracker* server) :
    Client(pid, server)
{
    connect(this, &Client::positionChanged, [this]() {
        setProgress(-1);
    });
    
    connect(this, &Pilot::progressChanged, [this]() {
        m_etaCalculated = false;
        emit etaChanged();
    });
}

Pilot::~Pilot() {}

QList<LonLat> Pilot::nodes(NodeSelection selection) const
{
    QList<LonLat> nodes;

    if (m_routeParserStatus < 2) {
        if (m_routeParserStatus == 0) {
            m_routeParserStatus = 1;
            QRunnable* task = new RouteParserTask(const_cast<Pilot*>(this));
            QThreadPool::globalInstance()->start(task);
        }

        if (selection.testFlag(DepartureToPilot) && departure()->isValid())
            nodes.append(departure()->position());

        nodes.append(position());

        if (selection.testFlag(PilotToDestination) && destination()->isValid())
            nodes.append(destination()->position());
    } else {
        if (selection.testFlag(DepartureToPilot))
            nodes.append(std::get<0>(m_nodes));

        nodes.append(position());

        if (selection.testFlag(PilotToDestination))
            nodes.append(std::get<1>(m_nodes));
    }

    return nodes;
}

void Pilot::setEta(const QTime& eta)
{
    if (m_eta != eta) {
        m_eta = eta;
        emit etaChanged();
        
        m_etaCalculated = m_eta.isValid();
    }
}

void Pilot::setProgress(int progress)
{
    if (m_progress != progress) {
        m_progress = progress;
        emit progressChanged();
    }
}

void Pilot::setAltitude(int altitude)
{
    if (m_altitude != altitude) {
        m_altitude = altitude;
        emit altitudeChanged(m_altitude);
    }
}

void Pilot::setGroundSpeed(int groundSpeed)
{
    if (m_groundSpeed != groundSpeed) {
        m_groundSpeed = groundSpeed;
        emit groundSpeedChanged(m_groundSpeed);
    }
}

void Pilot::setSquawk(const QString& squawk)
{
    if (m_squawk != squawk) {
        m_squawk = squawk;
        emit squawkChanged(m_squawk);
    }
}

void Pilot::setAircraft(const QString& aircraft)
{
    if (m_aircraft != aircraft) {
        m_aircraft = aircraft;
        emit aircraftChanged(m_aircraft);
    }
}

void Pilot::setTas(int tas)
{
    if (m_tas != tas) {
        m_tas = tas;
        emit tasChanged(m_tas);
    }
}

void Pilot::setRemarks(const QString& remarks)
{
    if (m_remarks != remarks) {
        m_remarks = remarks;
        emit remarksChanged(m_remarks);
    }
}

void Pilot::setStd(const QTime& std)
{
    if (m_std != std) {
        m_std = std;
        emit stdChanged(m_std);
    }
}

void Pilot::setSta(const QTime& sta)
{
    if (m_sta != sta) {
        m_sta = sta;
        emit staChanged(m_sta);
    }
}

void Pilot::setAtd(const QTime& atd)
{
    if (m_atd != atd) {
        m_atd = atd;
        emit atdChanged(m_atd);
    }
}

void Pilot::setHeading(int heading)
{
    if (m_heading != heading) {
        m_heading = heading;
        emit headingChanged(m_heading);
    }
}

void Pilot::setFlightRules(Pilot::FlightRules flightRules)
{
    if (m_flightRules != flightRules) {
        m_flightRules = flightRules;
        emit flightRulesChanged(m_flightRules);
    }
}

void Pilot::setFlightPhase(Pilot::FlightPhase flightPhase)
{
    if (m_flightPhase != flightPhase) {
        m_flightPhase = flightPhase;
        emit flightPhaseChanged(m_flightPhase);
    }
}

void Pilot::setDeparture(AirportObject* departure)
{
    if (m_departure != departure) {
        m_departure = departure;
        emit departureChanged(departure);
    }
}

void Pilot::setDestination(AirportObject* destination)
{
    if (m_destination != destination) {
        m_destination = destination;
        emit destinationChanged(destination);
    }
}

void Pilot::setCruiseAltitude(const QString& cruiseAltitude)
{
    if (m_cruiseAltitude != cruiseAltitude) {
        m_cruiseAltitude = cruiseAltitude;
        emit cruiseAltitudeChanged(m_cruiseAltitude);
    }
}

void Pilot::setPlannedTas(int plannedTas)
{
    if (m_plannedTas != plannedTas) {
        m_plannedTas = plannedTas;
        emit plannedTasChanged(plannedTas);
    }
}

void Pilot::setRoute(const QString& route)
{
    if (m_route != route) {
        m_route = route;
        emit routeChanged(m_route);
    }
}

void Pilot::setAirline(const Airline& airline)
{
    if (m_airline != airline) {
        m_airline = airline;
        emit airlineChanged(m_airline);
    }
}

void Pilot::calculateEta() const
{
    if (flightPhase() == Departing) {
        m_eta = m_sta;
    } else if (flightPhase() == Arrived) {
        m_eta = QDateTime::currentDateTimeUtc().time();
    } else if (destination()->isValid()) {
        qreal dist = nmDistance(position(), m_destination->position());
        int secs = static_cast<int>((dist / static_cast<qreal>(groundSpeed())) * 60.0 * 60.0);
        m_eta = QDateTime::currentDateTimeUtc().time().addSecs(secs);
    } else {
        m_eta = m_sta;
    }
    
    m_etaCalculated = true;
}

void Pilot::calculateProgress() const
{
    if (flightPhase() == Departing) {
        m_progress = 0;
    } else if (flightPhase() == Arrived) {
        m_progress = 100;
    } else {
        if (!departure()->position().isNull() && !destination()->position().isNull()) {
            qreal total = nmDistance(departure()->position(), destination()->position());
            qreal left = nmDistance(position(), destination()->position());
            m_progress = static_cast<int>(100.0 - (100.0 * left / total));
        } else {
            m_progress = 0;
        }
    }
}

void Pilot::setNodes(QList<LonLat> nodes)
{
    int pos = 0;
    if (nodes.size() > 2) {
        if (nodes.first().x() < position().x()) {
            while (nodes.at(pos).x() < position().x())
                ++pos;
        } else {
            while (nodes.at(pos).x() > position().x())
                ++pos;
        }
    } else if (nodes.size() == 2) {
        pos = 1;
    }

    m_nodes = std::make_tuple(nodes.mid(0, pos), nodes.mid(pos));
    emit nodesUpdated();
}

void Pilot::RouteParserTask::run()
{
    if (!m_pilot->departure()->isValid() || !m_pilot->destination()->isValid())
        return;

    qDebug() << "Parsing route for" << m_pilot->callsign();

    QList<LonLat> nodes;
    nodes.append(m_pilot->departure()->position());

    NatParser np(m_pilot->route());
    if (np.natFound())
        nodes.append(np.waypoints());

    nodes.append(m_pilot->destination()->position());

    m_pilot->m_routeParserStatus = 2;
    m_pilot->setNodes(nodes);
}

}} /* namespace Vatsinator::Core */
