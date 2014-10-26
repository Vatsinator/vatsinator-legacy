/*
 * abstractbookingprovider.h
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

#ifndef ABSTRACTBOOKINGPROVIDER_H
#define ABSTRACTBOOKINGPROVIDER_H

#include <QObject>
#include <QString>

class AtcBookingTableModel;

/**
 * The AbstractBookingProvider class is an abstraction for all services
 * that provide list of booked positions.
 */
class AbstractBookingProvider : public QObject {
   Q_OBJECT

public:
  /**
   * The default constructor passes _parent_ to QObject.
   */
  explicit AbstractBookingProvider(QObject *parent = nullptr);
  
  /**
   * This function returns a new model containing all bookings for the
   * given FIR. If there are no bookings, this method returns an empty model.
   */
  virtual AtcBookingTableModel* bookings(const QString& icao) = 0;
  
};

#endif // ABSTRACTBOOKINGPROVIDER_H
