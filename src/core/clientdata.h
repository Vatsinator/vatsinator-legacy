/*
 * clientdata.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_CLIENTDATA_H
#define CORE_CLIENTDATA_H

#include "coreexport.h"
#include "core/client.h"
#include <QSharedDataPointer>

namespace Vatsinator { namespace Core {

class Atc;
class Pilot;
class ClientDataData; // stupid name, I know

/**
 * \ingroup Core
 * @{
 * 
 * The ClientData is a container class for a raw client data
 * that is extracted from the VATSIM's data file.
 */
class __VtrCoreApi__ ClientData {
public:
    /**
     * Creates and empty (and invalid) instance.
     */
    ClientData();
    
    /**
     * Creates a ClientData instance that is a copy of \c other.
     */
    ClientData(const ClientData& other);
    
    /**
     * Destructor.
     */
    virtual ~ClientData();
    
    /**
     * Assigns \c other to this one.
     */
    ClientData& operator=(const ClientData& other);

    /**
     * Retrieves the client's callsign.
     */
    QString callsign() const;
    
    /**
     * Sets the client's callsign to the given value.
     */
    void setCallsign(const QString& callsign);
    
    /**
     * Retrieves the client's PID.
     */
    quint32 pid() const;
    
    /**
     * Sets the client's PID to the given value.
     */
    void setPid(quint32 pid);
    
    /**
     * Retrieves a value indicating whether the document is valid or not.
     */
    bool valid() const;
    
    /**
     * Specifies whether the document is valid or not.
     */
    void setValid(bool valid);
    
    /**
     * Retrieves the whole line; more information can be extracted from it.
     */
    QStringList line() const;
    
    /**
     * Sets the line to the given value.
     */
    void setLine(const QStringList& line);
    
    /**
     * Creates a new \c Client instance from the data.
     */
    Core::Client* createClient(ServerTracker* server);
    
    /**
     * Updates properties of the provided client.
     */
    void updateClient(Core::Client* client);
    
    /**
     * Creates a new ClientData instance from the raw, signle line in the
     * WhazzUp format.
     */
    static ClientData fromWhazzUpLine(const QString& line);

private:
    void updateClientImpl(Core::Client* client);
    void updateAtcImpl(Core::Atc* atc);
    void updatePilotImpl(Core::Pilot* pilot);
    
    QSharedDataPointer<ClientDataData> d;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_CLIENTDATA_H
