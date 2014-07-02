/*
    client.h
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


#ifndef CLIENT_H
#define CLIENT_H

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QObject>

#include "vatsimdata/lonlat.h"

class Client : public QObject {
  
  Q_OBJECT

  /*
   * Inherited by Pilot and Controller classes.
   */
  
signals:
  void updated();

public:
  
  /**
   * Prevent from creating foo-clients.
   */
  Client() = delete;
  
  /**
   * The data list. This can be, for example, one line
   * obtained from vatsim data servers, divided with ":".
   * For more info see client.cpp file.
   */
  Client(const QStringList&);
  
  /**
   * Update the client from the _data_ given.
   * The _data_ is just an appropriate line from the data file, which
   * is fetched from the internet.
   */
  virtual void update(const QStringList&);
  
  /**
   * Checks whether the client is still online or not.
   * This method simply compares the local copy of timestamp with the current timestamp
   * of VatsimDataHandler instance.
   * @return True if the client was present in the last data update, otherwise false.
   */
  bool isOnline() const;
  
  virtual ~Client() = default;

  /**
   * The client's Vatsim PID.
   */
  inline unsigned pid() const { return __pid; }
  
  /**
   * The client's callsign.
   */
  inline const QString& callsign() const { return __callsign; }
  
  /**
   * The client's real name.
   */
  inline const QString& realName() const { return __realName; }
  
  /**
   * The server that the client is connected to.
   */
  inline const QString& server() const { return __server; }
  
  /**
   * When the client went online.
   */
  inline const QDateTime& onlineFrom() const { return __onlineFrom; }
  
  /**
   * The current client's position.
   */
  inline const LonLat& position() const { return __position; }

protected:
  void setPosition(const LonLat&);
  
private:
  /* Client data */
  unsigned              __pid;
  QString               __callsign;
  QString               __realName;
  QString               __server;
  QDateTime             __onlineFrom;
  LonLat                __position;
  
  qint64                __timestamp;

};

#endif // CLIENT_H
