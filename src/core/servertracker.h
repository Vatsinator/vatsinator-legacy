/*
 * servertracker.h
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

#ifndef CORE_SERVERTRACKER_H
#define CORE_SERVERTRACKER_H

#include "core/airportmodel.h"
#include "core/clientmodel.h"
#include "core/firmodel.h"
#include "core/metarlistmodel.h"
#include "core/vtrcore_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The ServerTracker abstract class represents data provided by a VATSIM-like
 * server.
 *
 *
 */
class VTRCORE_EXPORT ServerTracker : public QObject {
    Q_OBJECT

    Q_PROPERTY(ClientModel* clients READ clients NOTIFY clientModelChanged)
    Q_PROPERTY(AirportModel* airports READ airports NOTIFY airportModelChanged)
    Q_PROPERTY(FirModel* firs READ firs NOTIFY firModelChanged)
    Q_PROPERTY(MetarListModel* metars READ metars NOTIFY metarModelChanged)

signals:
    void clientModelChanged(ClientModel* newModel);
    void airportModelChanged(AirportModel* newModel);
    void firModelChanged(FirModel* newModel);
    void metarModelChanged(MetarListModel* newModel);

public:
    ServerTracker(QObject* parent = nullptr);

    ClientModel* clients() { return m_clients; }
    const ClientModel* clients() const { return m_clients; }

    AirportModel* airports() { return m_airports; }
    const AirportModel* airports() const { return m_airports; }

    FirModel* firs() { return m_firs; }
    const FirModel* firs() const { return m_firs; }

    MetarListModel* metars() { return m_metars; }
    const MetarListModel* metars() const { return m_metars; }

protected:
    void setClientModel(ClientModel* clients);
    void setAirportModel(AirportModel* airports);
    void setFirModel(FirModel* firs);
    void setMetarModel(MetarListModel* metars);

private:
    ClientModel* m_clients = nullptr;
    AirportModel* m_airports = nullptr;
    FirModel* m_firs = nullptr;
    MetarListModel* m_metars = nullptr;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::ServerTracker*)

#endif // CORE_SERVERTRACKER_H
