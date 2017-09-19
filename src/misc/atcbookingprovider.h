/*
 * atcbookingprovider.h
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#ifndef MISC_ATCBOOKINGPROVIDER_H
#define MISC_ATCBOOKINGPROVIDER_H

#include "misc/atcbookingreply.h"
#include <QtCore/QString>
#include <QtCore/QObject>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 *
 * An interface for booking provider.
 */
class AtcBookingProvider {
public:
    /**
     * Retrieves all bookings for the given ICAO code.
     */
    virtual const AtcBookingReply* fetchBookings(const QString& icao) = 0;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_INTERFACE(Vatsinator::Misc::AtcBookingProvider, "org.eu.vatsinator.v8rmisc.AtcBookingProvider")

#endif // MISC_ATCBOOKINGPROVIDER_H
