/*
 * airline.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_AIRLINE_H
#define CORE_AIRLINE_H

#include "coreexport.h"
#include <QMetaType>
#include <QSharedData>
#include <QString>

namespace Vatsinator { namespace Core {

class AirlineData;

/**
 * \ingroup Core
 * @{
 *
 * The Airline class represents a single airline.
 */
class __VtrCoreApi__ Airline {

public:
    /**
     * Creates an empty \c Airline entity.
     */
    Airline();
    
    /**
     * \param icao The ICAO code of the airline.
     */
    explicit Airline(const QString& icao);
    
    /**
     * Copy constructor.
     */
    Airline(const Airline& other);
    
    /**
     * The destructor.
     */
    virtual ~Airline();
    
    /**
     * Assigns \c other to this airline.
     */
    Airline& operator=(const Airline& other);
    
    /**
     * Returns the ICAO code of the airline.
     */
    QString icao() const;
    
    /**
     * Sets the ICAO code of the airline to the given value.
     */
    void setIcao(const QString& icao);
    
    /**
     * Returns the IATA code of the airline.
     */
    QString iata() const;
    
    /**
     * Sets the IATA code of the airline to the given value.
     */
    void setIata(const QString& iata);
    
    /**
     * Returns name of the airline.
     */
    QString name() const;
    
    /**
     * Sets the name of the airline to the new value.
     */
    void setName(const QString& name);

private:
    QSharedDataPointer<AirlineData> d;

}; /** @} */

/**
 * \relates Airline
 */
bool operator==(const Airline& a, const Airline& b);

/**
 * \relates Airline
 */
bool operator!=(const Airline& a, const Airline& b);

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::Airline)
Q_DECLARE_TYPEINFO(Vatsinator::Core::Airline, Q_MOVABLE_TYPE);

#endif // CORE_AIRLINE_H
