/*
 * vatbookbookingprovider.cpp
 * Copyright (C) 2014  Michal Garapich <michal@garapich.pl>
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

#include <QtCore>

#include "network/plaintextdownloader.h"
#include "vatsimdata/atcbooking.h"
#include "vatsimdata/models/atcbookingtablemodel.h"

#include "vatbookbookingprovider.h"

namespace {
  static const QUrl VatbookUrl =
    QStringLiteral("http://vatbook.euroutepro.com/servinfo.asp");
  static const int RefreshInterval = 15 * 60000; // 15 min
}

VatbookBookingProvider::VatbookBookingProvider(QObject* parent) :
    AbstractBookingProvider(parent),
    __nullModel(new AtcBookingTableModel(this)),
    __downloader(new PlainTextDownloader(nullptr, this)) {

  __timer.setTimerType(Qt::VeryCoarseTimer);
  connect(&__timer, &QTimer::timeout, this, &VatbookBookingProvider::__update);
  __timer.start(RefreshInterval);
  
  connect(__downloader, &PlainTextDownloader::finished, this, &VatbookBookingProvider::__parseData);
  
  __update();
}

AtcBookingTableModel*
VatbookBookingProvider::bookings(const QString& icao) {
  return __bookings.value(icao, __nullModel);
}

void
VatbookBookingProvider::__addAtc(const QStringList& rawData) {
  AtcBooking* booking = new AtcBooking;
  
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
  booking->setCallsign(rawData[0]);
  booking->setRealName(rawData[2]);
  booking->setBookedDay(QDate::fromString(rawData[16], "yyyyMMdd"));
  booking->setTimeFrom(QTime::fromString(rawData[37], "hhmm"));
  booking->setTimeTo(QTime::fromString(rawData[14], "hhmm"));
  QString icao = booking->callsign().section('_', 0, 0);
  
  if (!__bookings.contains(icao))
    __bookings[icao] = new AtcBookingTableModel(this);
  
  __bookings[icao]->add(booking);
}

void
VatbookBookingProvider::__clear() {
  qDeleteAll(__bookings);
  __bookings.clear();
}

void
VatbookBookingProvider::__update() {
  __downloader->fetch(VatbookUrl);
}

void
VatbookBookingProvider::__parseData(QString data) {
  if (data.isEmpty())
    return;
  
  bool clientsSection = false;
  QStringList lines = data.split('\n', QString::SkipEmptyParts);
  
  for (const QString& line: lines) {
    if (line.startsWith(';'))
      continue;
    
    if (line.startsWith('!')) {
      if (line.simplified() == "!CLIENTS:")
        clientsSection = true;
      else
        clientsSection = false;
      
      continue;
    }
    
    if (!clientsSection)
      continue;
    
    QStringList rawDdata = line.split(':');
    if (rawDdata[3] == "ATC")
      __addAtc(rawDdata);
  }
}
