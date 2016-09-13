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


#ifndef CORE_AIRPORT_H
#define CORE_AIRPORT_H

#include "coreexport.h"
#include "lonlat.h"
#include <QMetaType>
#include <QSharedData>
#include <QString>

namespace Vatsinator { namespace Core {

class AirportData;

/**
 * \ingroup Core
 * @{
 * 
 * The Airport class represents a single airport.
 * 
 * The airport contains basic information, such as ICAO code, position,
 * name, city and country it belongs to.
 */
class __VtrCoreApi__ Airport {

public:
    /**
     * Creates an invalid airport.
     */
    Airport();
    
    /**
     * Creates the Airport instance that has the
     * given ICAO.
     */
    explicit Airport(const QString& icao);
    
    /**
     * Constructs a copy of \c other.
     */
    Airport(const Airport& other);
    
    /**
     * Destroys the airport instance.
     */
    virtual ~Airport();
    
    /**
     * Assigns \c other to this airport.
     */
    Airport& operator=(const Airport& other);
    
    /**
     * Returns the ICAO code of this airport.
     */
    QString icao() const;
    
    /**
     * Returns the IATA code of this airport.
     */
    QString iata() const;
    
    /**
     * Sets the IATA code to the new value.
     */
    void setIata(const QString& iata);
    
    /**
     * Returns city of the airport's location.
     */
    QString city() const;
    
    /**
     * Sets the airport's city to the new value.
     */
    void setCity(const QString& city);
    
    /**
     * Returns country the airport belongs to.
     */
    QString country() const;
    
    /**
     * Sets the airport's country to the new value.
     */
    void setCountry(const QString& country);
    
    /**
     * Returns the official name of the airport.
     */
    QString name() const;
    
    /**
     * Sets the official airport's name to the new value.
     */
    void setName(const QString& name);
    
    /**
     * Returns the airport's position.
     */
    LonLat position() const;
    
    /**
     * Sets the airport's position to the new value.
     */
    void setPosition(const LonLat& position);
    
    /**
     * Returns the airport's altitude.
     */
    int altitude() const;
    
    /**
     * Sets the airport's altitude to the new value.
     */
    void setAltitude(int altitude);
    
    /**
     * Specifies whether the airport is a valid one or not.
     */
    bool isValid() const;
    
private:
    QSharedDataPointer<AirportData> d;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::Airport)
Q_DECLARE_TYPEINFO(Vatsinator::Core::Airport, Q_MOVABLE_TYPE);


#endif // CORE_AIRPORT_H
