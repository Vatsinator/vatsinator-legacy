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

UserInterface::UserInterface(QObject* _parent) :
    QObject(_parent) {}

bool UserInterface::event(QEvent* _e) {
  if (_e->type() == Event::Notification)
    return notificationEvent(dynamic_cast<NotificationEvent*>(_e));
  else
    return QObject::event(_e);
}

bool
UserInterface::notificationEvent(NotificationEvent* _event) {
  switch (_event->gravity()) {
    case NotificationEvent::Fatal:
      fatal(_event->message());
      vApp()->exit(1);
      break;
      
    case NotificationEvent::Warning:
      warning(_event->message());
      break;
  }
  
  return true;
}
