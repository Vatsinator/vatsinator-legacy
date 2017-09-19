/*
    client.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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


#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "core/lonlat.h"
#include "core/v8rcore_export.h"
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

class ServerTracker;

/**
 * \ingroup Core
 * @{
 * 
 * The Client is a base class for all clients.
 */
class V8RCORE_EXPORT Client : public QObject {
    Q_OBJECT
    Q_ENUMS(State)
    
    /**
     * The server this client belongs to.
     */
    Q_PROPERTY(Vatsinator::Core::ServerTracker* server READ server CONSTANT)
    
    /**
     * This property holds the client's Vatsim PID.
     */
    Q_PROPERTY(quint32 pid READ pid CONSTANT)
    
    /**
     * This property holds the client's callsign.
     */
    Q_PROPERTY(QString callsign READ callsign WRITE setCallsign NOTIFY callsignChanged)
    
    /**
     * This property holds the client's real name.
     */
    Q_PROPERTY(QString realName READ realName WRITE setRealName NOTIFY realNameChanged)
    
    /**
     * The \c serverName property keeps the server that the client is connected to.
     */
    Q_PROPERTY(QString serverName READ serverName WRITE setServerName NOTIFY serverNameChanged)
    
    /**
     * This property keeps date and time when the client went online.
     */
    Q_PROPERTY(QDateTime onlineFrom READ onlineFrom WRITE setOnlineFrom NOTIFY onlineFromChanged)
    
    /**
     * This property holds current client's position.
     */
    Q_PROPERTY(LonLat position READ position WRITE setPosition NOTIFY positionChanged)
    
signals:
    void callsignChanged(const QString& callsign);
    void realNameChanged(const QString& realName);
    void serverNameChanged(const QString& server);
    void onlineFromChanged(const QDateTime& onlineFrom);
    void positionChanged(const LonLat& position);
    
public:
    /**
     * Creates a new client with the given \c pid that belongs to \c server.
     * Passes \c server to the QObject's constructor.
     */
    explicit Client(quint32 pid, ServerTracker* server);
    
    /**
     * The destructor.
     */
    virtual ~Client();
    
    ServerTracker* server() { return m_server; }
    const ServerTracker* server() const { return m_server; }
    quint32 pid() const { return m_pid; }
    const QString& callsign() const { return m_callsign; }
    void setCallsign(const QString& callsign);
    const QString& realName() const { return m_realName; }
    void setRealName(const QString& realName);
    const QString& serverName() const { return m_serverName; }
    void setServerName(const QString& serverName);
    const QDateTime& onlineFrom() const { return m_onlineFrom; }
    void setOnlineFrom(const QDateTime& onlineFrom);
    const LonLat& position() const { return m_position; }
    void setPosition(const LonLat& position);
    
    Client() = delete;
    
private:
    ServerTracker* m_server;
    
    /* Client data */
    quint32 m_pid;
    QString m_callsign;
    QString m_realName;
    QString m_serverName;
    QDateTime m_onlineFrom;
    LonLat m_position;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::Client*)

#endif // CORE_CLIENT_H
