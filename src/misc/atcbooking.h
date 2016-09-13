/*
 * atcbooking.h
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

#ifndef MISC_ATCBOOKING_H
#define MISC_ATCBOOKING_H

#include "miscexport.h"
#include <QDate>
#include <QTime>
#include <QSharedDataPointer>
#include <QString>

namespace Vatsinator { namespace Misc {

class AtcBookingData;

/**
 * \ingroup Misc
 * @{
 * 
 */
class __VtrMiscApi__ AtcBooking {
public:
    /**
     * Creates an empty AtcBooking instance.
     */
    AtcBooking();
    
    /**
     * Creates an AtcBooking that has the given \c callsign assigned.
     */
    explicit AtcBooking(const QString& callsign);
    
    /**
     * Creates an AtcBooking that is the copy of \c orig.
     */
    AtcBooking(const AtcBooking& orig);
    
    /**
     * Destroys this \c AtcBooking instance.
     */
    virtual ~AtcBooking();
    
    /**
     * Assigns \c other to this \c AtcBooking instance.
     */
    AtcBooking& operator=(const AtcBooking& other);
    
    /**
     * Returns the ATC booking callsign.
     */
    QString callsign() const;
    
    /**
     * Sets the booking's callsign to the given value.
     */
    void setCallsign(const QString& callsign);
    
    QString realName() const;
    void setRealName(const QString& realName);
    
    QDate bookedDay() const;
    void setBookedDay(const QDate& bookedDay);
    
    QTime timeFrom() const;
    void setTimeFrom(const QTime& timeFrom);
    
    QTime timeTo() const;
    void setTimeTo(const QTime& timeTo);
    
    bool isTrainingSession() const;
    void setIsTrainingSession(bool isTrainingSession);

private:
    QSharedDataPointer<AtcBookingData> d;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_TYPEINFO(Vatsinator::Misc::AtcBooking, Q_MOVABLE_TYPE);

#endif // MISC_ATCBOOKING_H
