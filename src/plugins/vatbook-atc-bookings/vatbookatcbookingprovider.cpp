/*
 * vatbookatcbookingprovider.cpp
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

#include "vatbookatcbookingprovider.h"
#include "core/filedownloader.h"
#include <QtCore>
#include <functional>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace {
    constexpr auto VatbookBookingsUrl = "http://vatbook.euroutepro.com/servinfo.asp";
}


VatbookAtcBookingProvider::VatbookAtcBookingProvider(QObject* parent) :
    QObject(parent)
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VatbookAtcBookingProvider::downloadBookings);
    timer->start(15 * 60000); // 15 minutes
    downloadBookings();
}

const AtcBookingReply* VatbookAtcBookingProvider::fetchBookings(const QString& icao)
{
    AtcBookingReply* reply = new AtcBookingReply(icao, this);
    
    if (m_initialized) {
        QList<AtcBooking> bookings = m_bookings.values(icao);
        std::for_each(bookings.begin(), bookings.end(), std::bind(&AtcBookingReply::addBooking, reply, std::placeholders::_1));
        reply->setFinished(true);
    } else {
        m_pending.append(reply);
    }
    
    return reply;
}

void VatbookAtcBookingProvider::addBookingFromRawData(const QStringList& data)
{
    /*
     * 0 Callsign
     * 1 Pid
     * 2 Real name
     * 3 Type
     * 4 Session type
     * 5
     * ...
     * 13
     * 14 Booked to
     * 15
     * 16 Date booked
     * 17
     * ...
     * 36
     * 37 Booked from
     */
    
    QString icao = data[0].section('_', 0, 0);
    AtcBooking booking(icao);
    booking.setCallsign(data[0]);
    booking.setRealName(data[2]);
    booking.setBookedDay(QDate::fromString(data[16], "yyyyMMdd"));
    booking.setTimeFrom(QTime::fromString(data[37], "hhmm"));
    booking.setTimeTo(QTime::fromString(data[14], "hhmm"));
    
    m_bookings.insert(icao, booking);
}

void VatbookAtcBookingProvider::finishRequests()
{
    for (AtcBookingReply* reply: m_pending) {
        QList<AtcBooking> bookings = m_bookings.values(reply->icao());
        std::for_each(bookings.begin(), bookings.end(), std::bind(&AtcBookingReply::addBooking, reply, std::placeholders::_1));
        reply->setFinished(true);
    }
    
    m_pending.clear();
}

void VatbookAtcBookingProvider::downloadBookings()
{
    FileDownloader* fd = new FileDownloader(this);
    connect(fd, &FileDownloader::finished, this, &VatbookAtcBookingProvider::parseBookings);
    fd->fetch(QUrl(VatbookBookingsUrl));
}

void VatbookAtcBookingProvider::parseBookings()
{
    FileDownloader* fd = qobject_cast<FileDownloader*>(sender());
    Q_ASSERT(fd);
    
    m_bookings.clear();
    
    QStringList lines = QString(fd->data()).split('\n', QString::SkipEmptyParts);
    bool isInClientsSection = false;
    
    for (const QString& line: lines) {
        if (line.startsWith(';'))
            continue;
        
        if (line.startsWith(QStringLiteral("!"))) {
            if (line.contains(QStringLiteral("CLIENTS")))
                isInClientsSection = true;
            else
                isInClientsSection = false;
            
            continue;
        }
        
        if (!isInClientsSection)
            continue;
        
        addBookingFromRawData(line.split(':'));
    }
    
    m_initialized = true;
    qDebug() << "VATBOOK: read" << m_bookings.size() << "bookings";
    
    finishRequests();
}
