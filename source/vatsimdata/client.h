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
 * The Client is a base class for all clients.
 */
class Client : public QObject {
  Q_OBJECT
  
  /**
   * This property holds the client's Vatsim PID.
   */
  Q_PROPERTY(unsigned pid READ pid NOTIFY updated)
  
  /**
   * This property holds the client's callsign.
   */
  Q_PROPERTY(QString callsign READ callsign NOTIFY updated)
  
  /**
   * This property holds the client's real name.
   */
  Q_PROPERTY(QString realName READ realName NOTIFY updated)
  
  /**
   * The _server_ property keeps the server that the client is connected to.
   */
  Q_PROPERTY(QString server READ server NOTIFY updated)
  
  /**
   * This property keeps date and time when the client went online.
   */
  Q_PROPERTY(QDateTime onlineFrom READ onlineFrom NOTIFY updated)
  
  /**
   * This property holds current client's position.
   */
  Q_PROPERTY(LonLat position READ position NOTIFY updated)
  
signals:
  /**
   * The update() signal is updated when the clients receives new data.
   */
  void updated();
  
  /**
   * The invalid() signal is emitted when the client logs out and this instance
   * will be deleted soon.
   * \note We cannot use destroyed() signal because sometimes we still need
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
   * Creates new Client from the data list. This can be, for example, one line
   * obtained from vatsim data servers, divided with ":".
   */
  Client(const QStringList& data);
  
  virtual ~Client();
  
  /**
   * Updates the client from the _data_ given.
   * The _data_ is just an appropriate line from the data file, which
   * is fetched from the internet.
   */
  virtual void update(const QStringList& data);
  
  /**
   * Marks this client as invalid; emits the invalid() signal.
   * \note This function should not be called if you do not really know
   * what you're doing.
   */
  virtual void invalidate();
  
  /**
   * Checks whether the client is still online or not.
   * This method simply compares the local copy of timestamp with the current timestamp
   * of VatsimDataHandler instance.
   * \return True if the client was present in the last data update, otherwise false.
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

  
  inline unsigned pid() const { return __pid; }
  inline const QString& callsign() const { return __callsign; }
  inline const QString& realName() const { return __realName; }
  inline const QString& server() const { return __server; }
  inline const QDateTime& onlineFrom() const { return __onlineFrom; }
  inline const LonLat& position() const { return __position; }

protected:
  void setPosition(const LonLat& position);
  
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
