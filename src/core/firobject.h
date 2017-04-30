/*
 * firobject.h
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_FIROBJECT_H
#define CORE_FIROBJECT_H

#include "core/clientlist.h"
#include "core/fir.h"
#include "core/vtrcore_export.h"
#include <QtCore/QMetaType>
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

class AirportObject;

/**
 * \ingroup Core
 * @{
 * 
 * An client-aggregator for FIRs.
 */
class VTRCORE_EXPORT FirObject : public ClientList {
    Q_OBJECT
    
    /**
     * ICAO code of the FIR's.
     */
    Q_PROPERTY(QString icao READ icao CONSTANT)
    
    /**
     * FIR's name (i. e. "Gander Center").
     */
    Q_PROPERTY(QString name READ name CONSTANT)
    
    /**
     * FIR's country.
     */
    Q_PROPERTY(QString country READ country CONSTANT)
    
    /**
     * Position of the FIR label.
     */
    Q_PROPERTY(Vatsinator::Core::LonLat labelPosition READ labelPosition CONSTANT)
    
    /**
     * Specifies whether the FIR is oceanic or not.
     */
    Q_PROPERTY(bool oceanic READ isOceanic CONSTANT)
    
    /**
     * FIR boundaries.
     */
    Q_PROPERTY(QList<Vatsinator::Core::Fir::Boundaries> boundaries READ boundaries CONSTANT)

public:
    /**
     * Creates a new FirObject. Passes \c parent to the QObject.
     */
    FirObject(const Fir& fir, QObject* parent = nullptr);
    
    QString icao() const { return m_fir.icao(); }
    QString name() const { return m_fir.name(); }
    QString country() const { return m_fir.country(); }
    LonLat labelPosition() const { return m_fir.labelPosition(); }
    bool isOceanic() const { return m_fir.isOceanic(); }
    QList<Fir::Boundaries> boundaries() const { return m_fir.boundaries(); }
    
private:
    /**
     * Returns \c true if the given \c airport belongs to this \c fir.
     */
    bool belongs(AirportObject* airport);
    
private slots:
    void trackAirport(AirportObject* airport);
    
private:
    Fir m_fir;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(const Vatsinator::Core::FirObject*)

#endif // CORE_FIROBJECT_H
