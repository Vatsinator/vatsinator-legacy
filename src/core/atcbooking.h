/*
 * atcbooking.h
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

#ifndef CORE_ATCBOOKING_H
#define CORE_ATCBOOKING_H

#include "coreexport.h"
#include <QDate>
#include <QSharedData>
#include <QString>

namespace Vatsinator { namespace Core {

class AtcBookingData;

/**
 * \ingroup Core
 * @{
 *
 * 
 */
class __VtrCoreApi__ AtcBooking {

public:
    AtcBooking();
    explicit AtcBooking(const QString& callsign);
    AtcBooking(const AtcBooking& other);
    virtual ~AtcBooking();
    
    QString callsign() const;
    void setCallsign(const QString& callsign);
    QString realName() const;
    void setRealName(const QString& realName);
    bool isTrainingSession() const;
    void setIsTrainingSession(bool isTraningSession);
    QDate bookedDay() const;
    void setBookedDay(const QDate& bookedDay);
    QTime timeFrom() const;
    void setTimeFrom(const QTime& timeFrom);
    QTime timeTo() const;
    void setTimeTo(const QTime& timeTo);
    
private:
    QSharedDataPointer<AtcBookingData> d;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_ATCBOOKING_H
