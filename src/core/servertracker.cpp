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
#include "atctracker.h"
#include "firlistreader.h"
#include "flighttracker.h"
#include "geo.h"
#include "resourcefile.h"
#include "uirlistreader.h"
#include "vatsimdatareader.h"
#include "vatsimstatusreader.h"
#include <QtCore>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Core {

/* The ServerTracker stores some internal values in Client' dynamic properties. */
constexpr auto UpdateNoKey = "serverTrackerUpdateNo";


ServerTracker::ServerTracker(QObject* parent) :
    QObject(parent),
    m_airlines(new AirlineListReader(this)),
    m_airports(new AirportListReader(this)),
    m_firDb(new FirListReader(this)),
    m_uirs(new UirListReader(this)),
    m_aliases(new AliasListReader(this)),
    m_metarManager(new MetarManager(this))
{
    connect(&m_timer, &QTimer::timeout, this, &ServerTracker::fetchData);
    
    for (auto f: m_firDb->firs())
        m_firs.insertMulti(f.icao(), new FirObject(f, this));

    m_airportObjects.insert(QString(), new AirportObject(QString(), this));
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
        new FlightTracker(p, this);
    } else if (Atc* a = qobject_cast<Atc*>(client)) {
        new AtcTracker(a, this);
    }
    
    emit clientAdded(client);
}

void ServerTracker::trackServer(const QUrl& statusUrl)
{
    VatsimStatusReader* reader = new VatsimStatusReader(this);
    connect(reader, &VatsimStatusReader::statusRead, this, &ServerTracker::readStatus);
    reader->read(statusUrl);
    m_isTracking = true;
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
    if (m_isTracking)
        fetchData();
    else
        qWarning("ServerTracker: unable to refresh in non-tracking mode");
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

AirportObject* ServerTracker::airportObject(const Airport& ap)
{
    Q_ASSERT(ap.isValid());
    
    if (!m_airportObjects.contains(ap.icao())) {
        AirportObject* a = new AirportObject(ap, this);
        m_airportObjects.insert(ap.icao(), a);
        emit airportAdded(a);
    }
    
    return m_airportObjects.value(ap.icao());
}

AirportObject* ServerTracker::airportObject(const QString& icao)
{
    if (icao.isEmpty())
        return m_airportObjects[icao];

    Airport ap = m_airports->findByIcao(icao);
    if (ap.isValid())
        return airportObject(ap);
    
    auto aliases = m_aliases->aliases(icao);
    for (auto alias: aliases) {
        ap = m_airports->findByIcao(alias);
        if (ap.isValid()) {
            return airportObject(ap);
        }
    }
    
    if (!m_airportObjects.contains(icao))
        m_airportObjects.insert(icao, new AirportObject(icao, this));
    
    Q_ASSERT(m_airportObjects.contains(icao));
    return m_airportObjects[icao];
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

void ServerTracker::readStatus(VatsimStatusDocument status)
{
    if (!status.isValid()) {
        // TODO Handle invalid status
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

    if (m_isTracking) {
        qDebug("Next data update in %d minutes", data.reload());
        int reload = data.reload() * 60 * 1000; /* min -> ms */
        m_timer.start(reload);
        emit dataFileDownloadFinished();
    }
    
    qDebug() << "Data file parsed; connected clients:" << data.connectedClients() << "; clients stored:" << m_clients.size();
}

}} /* namespace Vatsinator::Core */
