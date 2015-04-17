/*
 * atcbooking.h
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

#ifndef ATCBOOKING_H
#define ATCBOOKING_H

#include <QObject>
#include <QDate>
#include <QTime>

/**
 * The AtcBooking class represents a single ATC booking entry.
 * It contains all relevant information that should be displayed
 * in the view for the particular FIR or airport details window.
 * 
 * \sa AtcBookingTableModel.
 */
class AtcBooking : public QObject {
  Q_OBJECT
  
  /**
   * This property holds the booked ATC's callsign.
   */
  Q_PROPERTY(QString callsign READ callsign WRITE setCallsign)
  
  /**
   * This property holds the booker's real name.
   */
  Q_PROPERTY(QString realName READ realName WRITE setRealName)
  
  /**
   * This property indicates whether the booked session is a training
   * one or not.
   */
  Q_PROPERTY(bool trainingSession READ isTrainingSession WRITE setTrainingSession)
  
  /**
   * This property holds the day that the booking was made for.
   */
  Q_PROPERTY(QDate bookedDay READ bookedDay WRITE setBookedDay)
  
  /**
   * This property holds the time when the user should log in.
   */
  Q_PROPERTY(QTime timeFrom READ timeFrom WRITE setTimeFrom)
  
  /**
   * This property holds the time when the user should log out.
   */
  Q_PROPERTY(QTime timeTo READ timeTo WRITE setTimeTo)

public:
  /**
   * Constructs a new AtcBooking class instance.
   * 
   * \param parent Forwarded to QObject's constructor.
   */
  explicit AtcBooking(QObject* parent = nullptr);
  
  void setCallsign(const QString& callsign);
  inline const QString& callsign() const { return __callsign; }
  
  void setRealName(const QString& realName);
  inline const QString& realName() const { return __realName; }
  
  void setTrainingSession(bool trainingSession);
  inline bool isTrainingSession() const { return __trainingSession; }
  
  void setBookedDay(const QDate& bookedDay);
  inline const QDate& bookedDay() const { return __bookedDay; }
  
  void setTimeFrom(const QTime& timeFrom);
  inline const QTime& timeFrom() const { return __timeFrom; }
  
  void setTimeTo(const QTime& timeTo);
  inline const QTime& timeTo() const { return __timeTo; }
  
private:
  QString __callsign;
  QString __realName;
  bool __trainingSession;
  QDate __bookedDay;
  QTime __timeFrom;
  QTime __timeTo;
  
};

#endif // ATCBOOKING_H
