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

/**
 * Base class for all clients (Pilots and ATCs).
 */
class Client : public QObject {
  
  Q_OBJECT
  
signals:
  
  /**
   * The update() signal is updated when the clients receives new data.
   */
  void updated();
  
  /**
   * The invalid() signal is emitted when the client logs out and this instance
   * will be deleted soon.
   * NOTE We cannot use destroyed() signal because sometimes we still need
   * access to the client data, which is not available when destroyed() signal
   * is emitted.
   */
  void invalid();

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
  
  virtual ~Client();
  
  /**
   * Update the client from the _data_ given.
   * The _data_ is just an appropriate line from the data file, which
   * is fetched from the internet.
   */
  virtual void update(const QStringList&);
  
  /**
   * Marks this client as invalid; emits the invalid() signal.
   * NOTE This function should not be called if you do not really know
   * what you're doing.
   */
  virtual void invalidate();
  
  /**
   * Checks whether the client is still online or not.
   * This method simply compares the local copy of timestamp with the current timestamp
   * of VatsimDataHandler instance.
   * @return True if the client was present in the last data update, otherwise false.
   */
  bool isOnline() const;
  
  /**
   * Checks whether VATSIM provided a correct client's position.
   * Sometimes the given position is like (0, 90) or (0, 0), which is obviously
   * wrong. It means that the actual client's position should be recognized
   * i.e. by parsing the route. Use setPosition() to fixup the client's
   * position coordinates.
   */
  bool hasValidPosition() const;

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
