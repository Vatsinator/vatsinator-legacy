/*
 * vatbookatcbookingprovider.h
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

#ifndef VATBOOKATCBOOKINGPROVIDER_H
#define VATBOOKATCBOOKINGPROVIDER_H

#include "misc/atcbookingprovider.h"
#include <QObject>
#include <QMultiMap>

/**
 * \defgroup euroute-notams VATBOOK ATC booking provider.
 * @{
 */
class VatbookAtcBookingProvider : public QObject, public Vatsinator::Misc::AtcBookingProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.vtrmisc.AtcBookingProvider" FILE "vatbookatcbookingprovider.json")
    Q_INTERFACES(Vatsinator::Misc::AtcBookingProvider)

public:
    explicit VatbookAtcBookingProvider(QObject* parent = nullptr);
    
    /**
     * \copydoc Vatsinator::Misc::AtcBookingProvider::fetchBookings
     */
    const Vatsinator::Misc::AtcBookingReply* fetchBookings(const QString& icao) override;
    
private:
    void addBookingFromRawData(const QStringList& data);
    void finishRequests();
    
private slots:
    void downloadBookings();
    void parseBookings();
    
private:
    bool m_initialized = false;
    QMultiMap<QString, Vatsinator::Misc::AtcBooking> m_bookings;
    QList<Vatsinator::Misc::AtcBookingReply*> m_pending;

};

#endif // VATBOOKATCBOOKINGPROVIDER_H
