/*
    bookedcontroller.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef BOOKEDCONTROLLER_H
#define BOOKEDCONTROLLER_H

#include <QDate>
#include <QTime>
#include <QString>
#include <QStringList>

/**
 * The BookedController class represents a single booking entry.
 * All bookings are kept together in BookedAtcTableModel.
 */
class BookedController {
  
public:
  /**
   * Constructs the BookedController from the given data.
   */
  BookedController(const QStringList& data);
  
  /**
   * Gets the airport/FIR ICAO code.
   */
  QString icao() const;
  
  /**
   * Gets the booked client's callsign.
   */
  inline const QString& callsign() const { return __callsign; }
  
  /**
   * Gets the booked client's real name.
   */
  inline const QString& realName() const { return __realName; }
  
  /**
   * Returns true if the booking is planned as a training session.
   */
  inline bool isTrainingSession() const { return __trainingSession; }
  
  /**
   * Returns the day for which the booking is made.
   */
  inline const QDate& dateBooked() const { return __dateBooked; }
  
  /**
   * Gets the time when the client should log in.
   */
  inline const QTime& bookedFrom() const { return __bookedFrom; }
  
  /**
   * Gets the time of the session ending.
   */
  inline const QTime& bookedTo() const { return __bookedTo; }
  
  BookedController() = delete;
  
private:
  QString __callsign;
  QString __realName;
  
  bool __trainingSession;
  
  QDate __dateBooked;
  QTime __bookedFrom;
  QTime __bookedTo;
  
};

#endif // BOOKEDCONTROLLER_H
