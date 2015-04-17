/*
 * vatbookbookingprovider.h
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

#ifndef VATBOOKBOOKINGPROVIDER_H
#define VATBOOKBOOKINGPROVIDER_H

#include <QMap>
#include <QTimer>

#include "plugins/bookingprovider.h"

class PlainTextDownloader;

/**
 * The VatbookBookingProvider class obtains bookings from the VATBOOK service.
 */
class VatbookBookingProvider : public QObject, public BookingProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.Vatsinator.BookingProvider")
    Q_INTERFACES(BookingProvider)

public:
  explicit VatbookBookingProvider(QObject *parent = nullptr);
  
  AtcBookingTableModel* bookings(const QString& icao) override;
  
private:
  void __addAtc(const QStringList& rawData);
  void __clear();
  
private slots:
  void __update();
  void __parseData();
  
private:
  QMap<QString, AtcBookingTableModel*> __bookings;
  AtcBookingTableModel* __nullModel;
  PlainTextDownloader* __downloader;
  QTimer __timer;
  
};

#endif // VATBOOKBOOKINGPROVIDER_H
