/*
 * airportobject.h
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

#ifndef CORE_AIRPORTOBJECT_H
#define CORE_AIRPORTOBJECT_H

#include "airport.h"
#include "clientlist.h"
#include "coreexport.h"
#include <QMetaType>
#include <QObject>
#include <QSet>
#include <QSharedPointer>

namespace Vatsinator { namespace Core {

class Pilot;
class ServerTracker;

/**
 * \ingroup Core
 * @{
 * 
 * The AirportObject class encapsulates the \c Airport. It enables
 * attaching flights and controllers to it as well as makes use of Qt
 * properties. Note that this class is just an aggregator one. To obtain
 * information, use \ref Airport and \ref AirportListReader classes.
 * 
 * \sa Airport and AirportListReader.
 */
class __VtrCoreApi__ AirportObject : public ClientList {
    Q_OBJECT
    
    Q_PROPERTY(Vatsinator::Core::ServerTracker* server READ server CONSTANT)
    
    /**
     * Stores the ICAO code of the airport.
     */
    Q_PROPERTY(QString icao READ icao CONSTANT)
    
    /**
     * Stores the IATA code of the airport.
     */
    Q_PROPERTY(QString iata READ iata CONSTANT)
    
    /**
     * Stores the city of the airport.
     */
    Q_PROPERTY(QString city READ city CONSTANT)
    
    /**
     * Stores the country of the airport.
     */
    Q_PROPERTY(QString country READ country CONSTANT)
    
    /**
     * Stores the name of the airport.
     */
    Q_PROPERTY(QString name READ name CONSTANT)
    
    /**
     * Stores the position of the airport.
     */
    Q_PROPERTY(QPointF position READ position CONSTANT)
    
    /**
     * Stores the altitude of the airport.
     */
    Q_PROPERTY(int altitude READ altitude CONSTANT)
    
    /**
     * Stores count of inbound flights.
     */
    Q_PROPERTY(int inboundFlightsCount READ inboundFlightsCount)
    
    /**
     * Stores count of outbound flights.
     */
    Q_PROPERTY(int outboundFlightsCount READ outboundFlightsCount)
    
    /**
     * Specifies whether the airport is valid or not.
     * If the value of this property is false, it means that there is no
     * matching airport in the database and therefore no data for it is
     * available.
     */
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    
public:
    /**
     * Creates a new AirportObject instance that takes its data from
     * the provided \c airport. Passes \c server to \c QObject.
     * 
     * \note The AirportObject is unable to track any changes in the underlying
     * \c Airport instance.
     */
    AirportObject(const Airport& airport, ServerTracker* server);
    
    /**
     * Creates a new AirportObject instance that knows only its
     * ICAO code; other properties are not known.
     */
    explicit AirportObject(const QString& icao, ServerTracker* server);
    
    ServerTracker* server() { return m_server; }
    const ServerTracker* server() const { return m_server; }
    QString icao() const { return m_airport.icao(); }
    QString iata() const { return m_airport.iata(); }
    QString city() const { return m_airport.city(); }
    QString country() const { return m_airport.country(); }
    QString name() const { return m_airport.name(); }
    QPointF position() const { return QPointF(m_airport.position()); }
    int altitude() const { return m_airport.altitude(); }
    bool isValid() const { return m_valid; }
    
    /**
     * Gets representative name, i.e. ICAO code + city if the airport
     * is valid, otherwise returns the ICAO code only.
     */
    Q_INVOKABLE QString representativeName() const;
    
    /**
     * Returns a list of outbound flights.
     */
    QList<const Pilot*> outboundFlights() const;
    
    /**
     * Returns a list of inbound flights.
     */
    QList<const Pilot*> inboundFlights() const;
    
    int inboundFlightsCount() const;
    int outboundFlightsCount() const;

private:
    Airport m_airport;
    bool m_valid;
    ServerTracker* m_server;

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(const Vatsinator::Core::AirportObject*)

#endif // CORE_AIRPORTOBJECT_H
