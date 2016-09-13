/*
 * roles.h
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

#ifndef MISC_ROLES_H
#define MISC_ROLES_H

#include <Qt>

namespace Vatsinator { namespace Misc {

enum {
    UserRole = Qt::UserRole,
    
    /* Clients */
    CallsignRole    /**< Client's callsign (QString) */,
    RealNameRole    /**< Client's real name (QString) */,
    TypeRole        /**< Client's type (QString, "ATC" or "PILOT" */,
    InstanceRole    /**< The instance (Client*) */,
    
    /* Flight */
    DepartureRole   /**< Departure airport (AirportObject*) */,
    DestinationRole /**< Destination airport (AirportObject*) */,
    AircraftRole    /**< Aircraft (QString */,
    
    /* ATC */
    FrequencyRole   /**< Frequency (QString) */,
    
    /* NOTAM */
    IcaoRole        /**< The NOTAM ICAO code */,
    IdentRole       /**< The NOTAM ident */,
    NotamRole       /**< The notice content */,
    
    /* Bookings */
    BookedDayRole   /**< Day booked (QString) */,
    BookedHoursRole /**< Hours booked (QString) */,
};

}} /* namespace Vatsinator::Misc */

#endif // MISC_ROLES_H
