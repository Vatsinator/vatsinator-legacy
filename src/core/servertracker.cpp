/*
 * servertracker.cpp
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

#include "servertracker.h"
#include "airlinelistreader.h"
#include "airportlistreader.h"
#include "aliaslistreader.h"
#include "atc.h"
#include "firlistreader.h"
#include "geo.h"
#include "resourcefile.h"
#include "uirlistreader.h"
#include "vatsimdatareader.h"
#include "vatsimstatusreader.h"
#include <QtCore>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Core {

/* Maximum distance from aircraft to the airport when aircraft is not airborne (in nm) */
constexpr qreal MaxAircraftToAirport = 2.0;

/* Maximum distance from ATC position to the airport it controls */
constexpr qreal MaxAtcToAirport = 0.5;

/* Below this ground speeed (in knots) flight will be considered departing/arrived */
constexpr int MinimumGroundSpeed = 50;

/* The ServerTracker stores some internal values in Pilots' dynamic properties. */
constexpr auto AirportsValidKey = "airportObjectsValid";
constexpr auto UpdateNoKey = "serverTrackerUpdateNo";


ServerTracker::ServerTracker(QObject* parent) :
    QObject(parent),
    m_airlines(new AirlineListReader(this)),
    m_airportDb(new AirportListReader(this)),
    m_firDb(new FirListReader(this)),
    m_uirDb(new UirListReader(this)),
    m_aliases(new AliasListReader(this)),
    m_metarManager(new MetarManager(this))
{
    connect(&m_timer, &QTimer::timeout, this, &ServerTracker::fetchData);
    
    for (auto f: m_firDb->firs())
        m_firs.insertMulti(f.icao(), new FirObject(f, this));

    m_airports.insert(QString(), new AirportObject(QString(), this));
}

void ServerTracker::addClient(Client* client)
{
    if (client->position().isNull()) {
        qWarning("%s has invalid position; not adding it this time", qPrintable(client->callsign()));
        client->deleteLater();
        return;
    }
    
    client->setProperty(UpdateNoKey, QVariant::fromValue(m_updateNo));
    m_clients.insert(client->callsign(), client);
    
    if (Pilot* p = qobject_cast<Pilot*>(client)) {
        addFlightImpl(p);
    } else if (Atc* a = qobject_cast<Atc*>(client)) {
        addAtcImpl(a);
    }
    
    emit clientAdded(client);
}

void ServerTracker::trackServer(const QUrl& statusUrl)
{
    VatsimStatusReader* reader = new VatsimStatusReader(this);
    connect(reader, &VatsimStatusReader::statusRead, this, &ServerTracker::readStatus);
    reader->read(statusUrl);
}

Client* ServerTracker::findClient(const QString& callsign)
{
    return m_clients.value(callsign, nullptr);
}

const Client* ServerTracker::findClient(const QString& callsign) const
{
    return m_clients.value(callsign, nullptr);
}

QList<Client*> ServerTracker::clients() const
{
    return m_clients.values();
}

QList<FirObject*> ServerTracker::firObjects()
{
    return m_firs.values();
}

int ServerTracker::clientCount() const
{
    return m_clients.count();
}

int ServerTracker::pilotCount() const
{
    return std::count_if(m_clients.constBegin(), m_clients.constEnd(), [](auto it) {
        return qobject_cast<Pilot*>(it);
    });
}

int ServerTracker::atcCount() const
{
    return std::count_if(m_clients.constBegin(), m_clients.constEnd(), [](auto it) {
        Atc* atc = qobject_cast<Atc*>(it);
        return atc && atc->facility() != Atc::Obs;
    });
}

int ServerTracker::observerCount()
{
    return std::count_if(m_clients.constBegin(), m_clients.constEnd(), [](auto it) {
        Atc* atc = qobject_cast<Atc*>(it);
        return atc && atc->facility() == Atc::Obs;
    });
}

void ServerTracker::setUpdatesEnabled(bool updatesEnabled)
{
    if (updatesEnabled != m_updatesEnabled) {
        if (!m_updatesEnabled && updatesEnabled) { // switches from false to true
            if (!m_timer.isActive())
                fetchData();
        }
        
        m_updatesEnabled = updatesEnabled;
        emit updatesEnabledChanged(m_updatesEnabled);
    }
}

void ServerTracker::refreshData()
{
    fetchData();
}

void ServerTracker::markOfflineClients()
{
    auto values = m_clients.values();
    std::for_each(values.begin(), values.end(), [this](auto client) {
        QVariant prop = client->property(UpdateNoKey);
        Q_ASSERT(prop.isValid());
        Q_ASSERT(prop.canConvert(QMetaType::Int));
        int val = prop.toInt();
        if (val != m_updateNo) {
            this->disconnect(client);
            m_clients.remove(client->callsign());
            client->deleteLater();
        }
    });
}

void ServerTracker::addFlightImpl(Pilot* flight)
{
    connect(flight, &Pilot::routeChanged, this, &ServerTracker::invalidateAirports);
    connect(flight, &Pilot::groundSpeedChanged, this, &ServerTracker::maintainClient);
    maintainFlightImpl(flight);
    
    flight->setAirline(m_airlines->findByIcao(flight->callsign().left(3)));
}

void ServerTracker::addAtcImpl(Atc* atc)
{
    maintainAtcImpl(atc);
}

void ServerTracker::maintainFlightImpl(Pilot* flight)
{
    // airport database empty, always airborne
    if (m_airportDb->isEmpty()) {
        flight->setFlightPhase(Pilot::Airborne);
        return;
    }
    
    // departure or destination airports not set, find them
    if (!airportObjectsValid(flight)) {
        discoverFlightAirportsImpl(flight);
        AirportObject* a = flight->departure();
        Q_ASSERT(a);
        a->add(flight);
        
        a = flight->destination();
        Q_ASSERT(a);
        a->add(flight);
    }
    
    // determine flight phase
    if (flight->groundSpeed() < MinimumGroundSpeed) {
        qreal d = nmDistance(flight->position(), flight->departure()->position());
        if (d < MaxAircraftToAirport) {
            flight->setFlightPhase(Pilot::Departing);
        } else {
            d = nmDistance(flight->position(), flight->destination()->position());
            if (d < MaxAircraftToAirport)
                flight->setFlightPhase(Pilot::Arrived);
            else
                flight->setFlightPhase(Pilot::Airborne);
        }
    } else {
        flight->setFlightPhase(Pilot::Airborne);
    }
}

void ServerTracker::maintainAtcImpl(Atc* atc)
{
    if (!airportObjectsValid(atc)) {
        switch (atc->facility()) {
            case Atc::Atis:
            case Atc::Del:
            case Atc::Gnd:
            case Atc::Twr:
            case Atc::Dep:
            case Atc::App: {
                AirportObject* a = nullptr;
                if (atc->icao.isEmpty()) {
                    Airport ap = m_airportDb->nearest(atc->position());
                    if (nmDistance(atc->position(), ap.position()) < MaxAtcToAirport) {
                        a = airportObject(ap);
                        atc->icao = a->icao();
                    } else {
                        qFatal("No airport for ATC (%s)", qPrintable(atc->callsign()));
                    }
                } else {
                    a = airportObject(atc->icao);
                }

                atc->setAirport(a);
                a->add(atc);
                atc->setFir(nullptr);
                break;
            }
            
            case Atc::Ctr:
            case Atc::Fss: {
                Q_ASSERT(!atc->icao.isEmpty());
                if (FirObject* f = firObject(atc->icao)) {
                    f->add(atc);
                    atc->setFir(f);
                } else {
                    bool found;
                    UirListReader::UirData uir;
                    std::tie(found, uir) = m_uirDb->find(atc->icao);
                    if (found) {
                        atc->setDescription(std::get<1>(uir));
                        atc->setIsUir(true);

                        for (const QString& icao: std::get<2>(uir)) {
                            FirObject* f = firObject(icao);
                            if (f)
                                f->add(atc);
                        }
                    } else {
                        qWarning("No FIR for ATC (%s)", qPrintable(atc->callsign()));
                    }
                }
                
                atc->setAirport(nullptr);
                break;
            }
            
            case Atc::Obs:
                atc->setAirport(nullptr);
                atc->setFir(nullptr);
                break;
        }
        
        setAirportObjectsValid(atc, true);
    }
}

void ServerTracker::discoverFlightAirportsImpl(Pilot* flight)
{
    AirportObject *dep = nullptr, *dest = nullptr;

    if (flight->plannedDepartureAirport.isEmpty()) { // departure airport not filled
        Airport ap = m_airportDb->nearest(flight->position());
        AirportObject* o = airportObject(ap);
        qreal d = nmDistance(flight->position(), o->position());
        if (d < MaxAircraftToAirport) {
            qDebug("Flight %s is at airport %s (nearest one)",
                   qPrintable(flight->callsign()), qPrintable(o->icao()));

            dep = o;
        }
    }

    if (!dep) {
        dep = airportObject(flight->plannedDepartureAirport);
    }
    
    if (!dest) {
        dest = airportObject(flight->plannedDestinationAirport);
    }
    
    flight->setDeparture(dep);
    flight->setDestination(dest);
    setAirportObjectsValid(flight, true);
}

AirportObject* ServerTracker::airportObject(const Airport& ap)
{
    Q_ASSERT(ap.isValid());
    
    if (!m_airports.contains(ap.icao())) {
        AirportObject* a = new AirportObject(ap, this);
        m_airports.insert(ap.icao(), a);
        emit airportAdded(a);
    }
    
    return m_airports.value(ap.icao());
}

AirportObject* ServerTracker::airportObject(const QString& icao)
{
    if (icao.isEmpty())
        return m_airports[icao];

    Airport ap = m_airportDb->findByIcao(icao);
    if (ap.isValid())
        return airportObject(ap);
    
    auto aliases = m_aliases->aliases(icao);
    for (auto alias: aliases) {
        ap = m_airportDb->findByIcao(alias);
        if (ap.isValid()) {
            return airportObject(ap);
        }
    }
    
    if (!m_airports.contains(icao))
        m_airports.insert(icao, new AirportObject(icao, this));
    
    Q_ASSERT(m_airports.contains(icao));
    return m_airports[icao];
}

FirObject* ServerTracker::firObject(const QString& icao)
{
    FirObject* fir = m_firs.value(icao, nullptr);
    if (!fir) {
        auto aliases = m_aliases->aliases(icao);
        for (auto it: aliases)  {
            fir = m_firs.value(it, nullptr);
            if (fir) 
                break;
        }
    }
    
    return fir;
}

bool ServerTracker::airportObjectsValid(const Client* client) const
{
    auto value = client->property(AirportsValidKey);
    if (!value.isValid())
        return false;
    
    Q_ASSERT(value.canConvert<bool>());
    return value.toBool();
}

void ServerTracker::setAirportObjectsValid(Client* client, bool valid)
{
    client->setProperty(AirportsValidKey, QVariant::fromValue(valid));
}

void ServerTracker::fetchData()
{
    m_timer.stop();
    
    if (!m_updatesEnabled)
        return;
    
    int serverNo = qrand() % m_status.dataFileUrls().length();
    QUrl dataUrl = m_status.dataFileUrls().at(serverNo);
    VatsimDataReader* reader = new VatsimDataReader(this);
    connect(reader, &VatsimDataReader::dataRead, this, &ServerTracker::readData);
    reader->read(dataUrl);
    emit dataFileDownloadStarted();
}

void ServerTracker::invalidateAirports()
{
    Client* client = qobject_cast<Client*>(sender());
    Q_ASSERT(client);
    
    if (Pilot* flight = qobject_cast<Pilot*>(sender())) {
        AirportObject* a = flight->departure();
        Q_ASSERT(a);
        a->remove(flight);
        
        a = flight->destination();
        Q_ASSERT(a);
        a->remove(flight);
    } else if (Atc* atc = qobject_cast<Atc*>(sender())) {
        AirportObject* a = atc->airport();
        if (a)
            a->remove(flight);
    } else {
        Q_UNREACHABLE();
    }
    
    setAirportObjectsValid(client, false);
}

void ServerTracker::maintainClient()
{
    Client* client = qobject_cast<Client*>(sender());
    Q_ASSERT(client);
    Q_ASSERT(m_clients.contains(client->callsign()));
    
    if (Pilot* flight = qobject_cast<Pilot*>(client)) {
        maintainFlightImpl(flight);
    } else if (Atc* atc = qobject_cast<Atc*>(client)) {
        maintainAtcImpl(atc);
    }
}

void ServerTracker::readStatus(VatsimStatusDocument status)
{
    if (!status.isValid()) {
        qWarning("STATUS INVALID! Handle me pls ;-;");
        return;
    }
    
    m_status = status;
    fetchData();
    
    QString url = m_status.metar().toString() % QStringLiteral("?id=ALL");
    m_metarManager->setUrl(QUrl(url));
}

void ServerTracker::readData(VatsimDataDocument data)
{
    m_updateNo += 1;
    m_lastUpdate = data.update();
    
    std::for_each(data.clients().begin(), data.clients().end(), [this](auto cd) {
        Client* client = this->findClient(cd.callsign());
        if (client) {
            cd.updateClient(client);
            client->setProperty(UpdateNoKey, QVariant::fromValue(m_updateNo));
        } else {
            this->addClient(cd.createClient(this));
        }
    });
    
    markOfflineClients();
    
    qDebug("Next data update in %d minutes", data.reload());
    int reload = data.reload() * 60 * 1000; /* min -> ms */
    m_timer.start(reload);
    
    qDebug() << "Data file parsed; connected clients:" << data.connectedClients() << "; clients stored:" << m_clients.size();
    emit dataFileDownloadFinished();
}

}} /* namespace Vatsinator::Core */
