/*
    userinterface.cpp
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

#include "events/notificationevent.h"
#include "vatsinatorapplication.h"

#include "userinterface.h"

UserInterface::UserInterface(QObject* parent) :
    QObject(parent) {}

bool UserInterface::event(QEvent* event) {
  if (event->type() == Event::Notification)
    return notificationEvent(dynamic_cast<NotificationEvent*>(event));
  else
    return QObject::event(event);
}

bool
UserInterface::notificationEvent(NotificationEvent* event) {
  switch (event->gravity()) {
    case NotificationEvent::Fatal:
      fatal(event->message());
      vApp()->exit(1);
      break;
      
    case NotificationEvent::Warning:
      warning(event->message());
      break;
  }
  
  return true;
}
