/*
    client.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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


#ifndef CLIENT_H
#define CLIENT_H

#include <QDateTime>
#include <QString>
#include <QStringList>

// types
enum ClientType {
  PILOT, ATC
};

class Client {

  /**
   * Abstract class for connected client.
   * Inherited by Pilot and Controller classes.
   */

public:
  Client() = delete;
  Client(const QStringList&);

  virtual ClientType type() const = 0;

  virtual ~Client() {}

  unsigned  pid;
  QString   callsign;
  QString   realName;

  QString   server;
  QDateTime onlineFrom;

  struct {
    float latitude;
    float longitude;
  } position;

};

#endif // CLIENT_H
