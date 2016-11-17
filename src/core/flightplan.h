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

    FlightPlan(const FlightPlan& other);

    virtual ~FlightPlan();

    FlightPlan& operator=(const FlightPlan& other);

    bool operator==(const FlightPlan& other) const;
    bool operator!=(const FlightPlan& other) const;

    bool isEmpty() const;

    QString route() const;
    void setRoute(const QString& route);
    QString departureAirport() const;
    void setDepartureAirpoirt(const QString& departureAirpoirt);
    QString destinationAirport() const;
    void setDestinationAirport(const QString& destinationAirport);

private:
    QSharedDataPointer<FlightPlanData> d;

};

}} /* namespace Vatsinator::Core */


Q_DECLARE_METATYPE(Vatsinator::Core::FlightPlan)
Q_DECLARE_TYPEINFO(Vatsinator::Core::FlightPlan, Q_MOVABLE_TYPE);

#endif // CORE_FLIGHTPLAN_H
