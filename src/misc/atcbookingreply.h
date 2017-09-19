/*
 * atcbookingreply.h
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

#ifndef MISC_ATCBOOKINGREPLY_H
#define MISC_ATCBOOKINGREPLY_H

#include "misc/atcbooking.h"
#include "misc/v8rmisc_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * A reply for AtcBooking request.
 */
class V8RMISC_EXPORT AtcBookingReply : public QObject {
    Q_OBJECT
    
    /**
     * Stores the ICAO code of airport or FIR.
     */
    Q_PROPERTY(QString icao READ icao)
    
signals:
    /**
     * Emitted when the booking is ready.
     */
    void finished();
    
public:
    /**
     * Creates a new \c AtcBookingReply instance.
     * \param icao An ICAO code of airport of FIR for which the booking list was requested.
     * \param parent Passed to QObject's constructor.
     */
    explicit AtcBookingReply(const QString& icao, QObject* parent = nullptr);
    
    /**
     * Returns the list of the ATC bookings.
     */
    const QList<AtcBooking>& bookings() const { return m_bookings; }
    
    /**
     * ATC \c booking to the list of bookings.
     */
    void addBooking(const AtcBooking& booking);
    
    /**
     * Specifies whether the NOTAM list is ready or not.
     */
    bool isFinished() const { return m_finished; }
    
    /**
     * Sets the \c finished to the given value.
     */
    void setFinished(bool finished);
    
    const QString& icao() const { return m_icao; }
    
private:
    bool m_finished = false;
    QString m_icao;
    QList<AtcBooking> m_bookings;

}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_ATCBOOKINGREPLY_H
