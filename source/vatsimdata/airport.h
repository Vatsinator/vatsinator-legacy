/*
    airport.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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


#ifndef AIRPORT_H
#define AIRPORT_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QList>

#include "vatsimdata/controller.h"

struct AirportRecord;
class AtcTableModel;
class Fir;
class FlightTableModel;
class Pilot;

/**
 * The Airport class represents a single airport.
 */
class Airport : public QObject {
    Q_OBJECT
    
    /**
     * The ICAO code of the airport.
     */
    Q_PROPERTY(QString icao READ icao CONSTANT)
    
    /**
     * The IATA code of the airport.
     */
    Q_PROPERTY(QString iata READ iata CONSTANT)
    
    /**
     * Position of the airport.
     */
    Q_PROPERTY(LonLat position READ position CONSTANT)
    
    /**
     * The closest city of the airport.
     */
    Q_PROPERTY(QString city READ city CONSTANT)
    
    /**
     * Country.
     */
    Q_PROPERTY(QString country READ country CONSTANT)
    
    /**
     * Name of the airport.
     */
    Q_PROPERTY(QString name READ name CONSTANT)
    
    /**
     * Altitude of the airport.
     */
    Q_PROPERTY(int altitude READ altitude CONSTANT)
    
    /**
     * Defines whether the airport was created from the
     * database record or as a placeholder for the unrecognized
     * one.
     */
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    
signals:
    /**
     * Emitted only when a flight is added or removed, either inbound
     * or outbound, or when ATC staff changes.
     */
    void updated();
    
public:
    /**
     * Creates the Airport instance, getting all its data
     * from the given record. This costructor is used only
     * by VatsimDataHandler.
     * 
     * \param data Record in the database.
     */
    Airport(const AirportRecord* record, QObject* parent = nullptr);
    
    /**
     * Creates the Airport instance that has the
     * given ICAO. The airport is invalid. It becomes
     * only a placeholder for the ICAO code.
     */
    Airport(const QString& icao, QObject* parent = nullptr);
    
    /**
     * Creates the placeholder Airport instance, without ICAO code.
     */
    Airport(QObject* parent = nullptr);
    
    /**
     * Counts flights that are about to take off.
     * \param includePrefiled If false, this method will not count prefiled flights.
     *    Default: true.
     */
    unsigned countDepartures(bool includePrefiled = true) const;
    
    /**
     * Counts flights that originate from the airport and are airborne
     * or have just arrived.
     */
    unsigned countOutbounds() const;
    
    /**
     * Counts flights that have just landed.
     */
    unsigned countArrivals() const;
    
    /**
     * Counts flights that fly towards the airport.
     */
    unsigned countInbounds() const;
    
    /**
     * Returns OR-combined facilities that are available on the airport.
     */
    Controller::Facilities facilities() const;
    
    /**
     * Adds new controller to the airport.
     */
    void addStaff(const Controller* atc);
    
    /**
     * Adds new inbound flight to the airport.
     */
    void addInbound(const Pilot* pilot);
    
    /**
     * Adds new outbound flight to the airport.
     */
    void addOutbound(const Pilot* pilot);
    
    /**
     * \return True if the airport does not have any controllers or flights, otherwise false.
     */
    bool isEmpty() const;
    
    /**
     * \return True if at least one ATC is available on the airport.
     */
    bool isStaffed() const;
    
    /**
     * \return The airport's position.
     */
    LonLat position() const;
    
    inline const QString& icao() const
    {
        return __icao;
    }
    
    inline const QString& iata() const
    {
        return __iata;
    }
    
    inline const QString& city() const
    {
        return __city;
    }
    
    inline const QString& country() const
    {
        return __country;
    }
    
    inline const QString& name() const
    {
        return __name;
    }
    
    inline int altitude() const
    {
        return __altitude;
    }
    
    inline bool isValid() const
    {
        return __data != nullptr;
    }
    
    /**
     * \return Staff model of the airport.
     */
    inline AtcTableModel* staff() const
    {
        return __staff;
    }
    
    /**
     * \return Inbound flights for the airport.
     */
    inline FlightTableModel* inbounds() const
    {
        return __inbounds;
    }
    
    /**
     * \return Outbound flights for the airport.
     */
    inline FlightTableModel* outbounds() const
    {
        return __outbounds;
    }
    
    ~Airport() = default;
    
private:
    const AirportRecord* __data;
    const QString __icao;
    const QString __iata;
    const QString __city;
    const QString __country;
    const QString __name;
    const int __altitude;
    
    AtcTableModel* __staff;
    FlightTableModel* __inbounds;
    FlightTableModel* __outbounds;
    QList<Fir*> __firs;
    
};

Q_DECLARE_METATYPE(const Airport*)

#endif // AIRPORT_H
