/*
 * flightplan.cpp
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_FLIGHTPLAN_H
#define CORE_FLIGHTPLAN_H

#include "coreexport.h"
#include <QMetaType>
#include <QSharedData>
#include <QString>

namespace Vatsinator { namespace Core {

class FlightPlanData;

/**
 * \ingroup Core
 * @{
 *
 * The FlightPlan class groups all flight plan related values.
 */
class __VtrCoreApi__ FlightPlan {
public:
    /**
     * Creates an invalid FlightPlan instance.
     */
    FlightPlan();

    /**
     * Creates a FlightPlan that is the copy of \c other.
     */
    FlightPlan(const FlightPlan& other);

    /**
     * Destroys this FlightPlan instance.
     */
    virtual ~FlightPlan();

    /**
     * Assigns \c other to this FlightPlan instance and returns reference
     * to it.
     */
    FlightPlan& operator=(const FlightPlan& other);

    /**
     * Compares this FlightPlan with \c other.
     * \return \c true if \c other exactly equals \c this.
     */
    bool operator==(const FlightPlan& other) const;

    /**
     * Compares this FlightPlan with \c other.
     * \return \c true if at least one property of \c other is different than
     * \c this.
     */
    bool operator!=(const FlightPlan& other) const;

    /**
     * Returns \c true if the flight plan is empty (i.e. not filled).
     */
    bool isEmpty() const;

    /**
     * Returns the flight route, as filled in the flight plan.
     */
    QString route() const;

    /**
     * Sets the flight route to the given value.
     */
    void setRoute(const QString& route);

    /**
     * Returns the departure airport ICAO code, as filled in the flight plan.
     */
    QString departureAirport() const;

    /**
     * Sets the departure airport ICAO code to the given value.
     */
    void setDepartureAirpoirt(const QString& departureAirpoirt);

    /**
     * Returns the destination airpoirt ICAO code, as filled in the flight plan.
     */
    QString destinationAirport() const;

    /**
     * Sets the destination airport ICAO code to the given value.
     */
    void setDestinationAirport(const QString& destinationAirport);

private:
    QSharedDataPointer<FlightPlanData> d;

};

}} /* namespace Vatsinator::Core */


Q_DECLARE_METATYPE(Vatsinator::Core::FlightPlan)
Q_DECLARE_TYPEINFO(Vatsinator::Core::FlightPlan, Q_MOVABLE_TYPE);

#endif // CORE_FLIGHTPLAN_H
