/*
 * notificationevent.h
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

#ifndef NOTIFICATIONEVENT_H
#define NOTIFICATIONEVENT_H

#include <QEvent>
#include <QString>

#include "events/types.h"

class NotificationEvent : public QEvent {

public:
  
  enum Gravity {
    
    /* Fatal errors */
    Fatal,
    
    /* Warnings */
    Warning,
    
    /* Just info */
    Information
    
  };
  
  /**
   * This constructor takes the notification gravity and the notification
   * message.
   */
  NotificationEvent(Gravity, QString);
  
  /**
   * Gets the notification gravity.
   */
  inline Gravity gravity() const { return __gravity; }
  
  /**
   * Gets the notification message.
   */
  inline const QString& message() const { return __message; }
  
private:
  Gravity __gravity;
  QString __message;
  
};

#endif // NOTIFICATIONEVENT_H
