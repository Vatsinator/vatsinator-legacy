/*
 * servertracker.h
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

#ifndef CORE_SERVERTRACKER_H
#define CORE_SERVERTRACKER_H

#include "airportobject.h"
#include "client.h"
#include "coreexport.h"
#include "firobject.h"
#include "pilot.h"
#include "metarmanager.h"
#include "vatsimdatadocument.h"
#include "vatsimstatusdocument.h"
#include <QMap>
#include <QList>
#include <QObject>
#include <QUrl>
#include <QTimer>

namespace Vatsinator { namespace Core {

class AirlineListReader;
class AirportListReader;
class AliasListReader;
class FirListReader;
class UirListReader;

/**
 * \ingroup Core
 * @{
 * 
 * The ServerTracker class tracks connected clients.
 */
class __VtrCoreApi__ ServerTracker : public QObject {
    Q_OBJECT
    
    /**
     * The \c updatesEnabled property specifies whether this \c ServerTracker
     * instance is allowed to VATSIM data updates.
     */
    Q_PROPERTY(bool updatesEnabled READ updatesEnabled WRITE setUpdatesEnabled NOTIFY updatesEnabledChanged)
    
    /**
     * The \c MetarManager instance.
     */
    Q_PROPERTY(Vatsinator::Core::MetarManager* metarManager READ metarManager CONSTANT)
    
    /**
     * The \c lastUpdate property keeps date and time VATSIM data was updated.
     */
    Q_PROPERTY(QDateTime lastUpdate READ lastUpdate NOTIFY dataFileDownloadFinished)
    
signals:
    /**
     * Emitted when a new client is added to this \c ServerTracker instance
     * and therefore tracked.
     */
    void clientAdded(Client* client);
    
    /**
     * Emitted when the airport object is added to the server.
     */
    void airportAdded(AirportObject* airport);
    
    /**
     * Emitted when the data file starts being downloaded.
     */
    void dataFileDownloadStarted();
    
    /**
     * Emitted when the data file is downloaded and successfully parsed.
     */
    void dataFileDownloadFinished();
    
    void updatesEnabledChanged(bool updatesEnabled);

public:
    /**
     * Creates a new ServerTracker instance. Passes \c parent to
     * the QObject's constructor.
     */
    explicit ServerTracker(QObject* parent = nullptr);
    
    /**
     * Starts tracking the client.
     * 
     * The server tracker takes ownership of the \c client. When the client
     * wents offline, it is deleted shortly aferwards. It is therefore adviced
     * against keeping the pointer after the client logs off.
     * 
     * \note You usually do not need to call this function manually.
     * \note The ServerTracker takes ownership of \c client.
     * 
     * \param client The client to be tracked.
     * \sa trackServer().
     */
    void addClient(Client* client);
    
    /**
     * Starts tracking the given server.
     * 
     * The \c statusUrl must be an URL of the status.txt that makes
     * the entry point for the \c ServerTracker.
     */
    void trackServer(const QUrl& statusUrl);
    
    /**
     * Looks for the client under the provided callsign.
     * 
     * \return Client with the specified callsign or \c nullptr if not found.
     * @{
     */
    Client* findClient(const QString& callsign);
    const Client* findClient(const QString& callsign) const;
    /** @} */

    /**
     * Returns a list of connected clients.
     */
    QList<Client*> clients() const;
    
    /**
     * Gives access to all FIR objects.
     */
    QList<FirObject*> firObjects();
    
    /**
     * Counts connected clients.
     */
    int clientCount() const;
    
    /**
     * Counts flights.
     */
    int pilotCount() const;
    
    /**
     * Counts ATCs.
     */
    int atcCount() const;
    
    /**
     * Counts observers.
     */
    int observerCount();
    
    bool updatesEnabled() const { return m_updatesEnabled; }
    void setUpdatesEnabled(bool updatesEnabled);
    MetarManager* metarManager() { return m_metarManager; }
    const MetarManager* metarManager() const { return m_metarManager; }
    const QDateTime& lastUpdate() const { return m_lastUpdate; }
    
public slots:
    /**
     * Refreshes VATSIM data immediately.
     */
    void refreshData();

    /**
     * Reads the data document.
     */
    void readData(VatsimDataDocument data);
    
private:
    /**
     * Checks if every client has the correct update number. The update number
     * is incremented with every data update. If the client has old update
     * number, it means that he went offline.
     */
    void markOfflineClients();
    
    /**
     * Does flight-specific actions when adding a client.
     */
    void addFlightImpl(Pilot* flight);
    
    /**
     * Does atc-specific actions when adding a client.
     */
    void addAtcImpl(Atc* atc);
    
    /**
     * Flight-specific actions to maintain a client.
     */
    void maintainFlightImpl(Pilot* flight);
    
    /**
     * Find (departure, destination) airports pair.
     */
    std::tuple<AirportObject*, AirportObject*> findAirports(const Pilot* flight);
    
    /**
     * Finds the airport object for the given Airport.
     */
    AirportObject* airportObject(const Airport& ap);
    
    /**
     * Finds the airport object for the given ICAO code.
     */
    AirportObject* airportObject(const QString& icao);
    
    /**
     * Finds the FIR object for the given ICAO code.
     */
    FirObject* firObject(const QString& icao);
    
private slots:
    /**
     * Picks a random data server URL and start downloading the data file.
     */
    void fetchData();
    
    /**
     * Removes any airport bindings from the sender client
     * instance in order to have it re-discovered.
     */
    void invalidateAirports();
    
    /**
     * Performs the client periodics checks:
     * * if the client wents offline, removes it;
     * * if the clinet is a pilot, checks whether its airport matches
     *      are up-to-date.
     */
    void maintainClient();
    
    /**
     * Reads the status.txt document.
     */
    void readStatus(VatsimStatusDocument status);
    
private:
    QMap<QString, Client*> m_clients;
    VatsimStatusDocument m_status;
    bool m_isTracking = false;
    int m_updateNo = 0;
    AirlineListReader* m_airlines;
    AirportListReader* m_airportDb;
    FirListReader* m_firDb;
    UirListReader* m_uirDb;
    AliasListReader* m_aliases;
    QMap<QString, AirportObject*> m_airports;
    QMap<QString, FirObject*> m_firs;
    QTimer m_timer;
    bool m_updatesEnabled = true;
    QDateTime m_lastUpdate;
    MetarManager* m_metarManager;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::ServerTracker*)

#endif // CORE_SERVERTRACKER_H
