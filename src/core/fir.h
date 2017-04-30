/*
 * fir.h
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

#ifndef CORE_FIR_H
#define CORE_FIR_H

#include "core/lonlat.h"
#include "core/vtrcore_export.h"
#include <QtCore/QMetaType>
#include <QtCore/QSharedData>
#include <QtCore/QVector>

namespace Vatsinator { namespace Core {

class FirData;

/**
 * \ingroup Core
 * @{
 * 
 * The \c Fir class represents a single FIR (Flight Information Region).
 */
class VTRCORE_EXPORT Fir {

public:
    /* List of points that make the boundaries of the FIR. */
    using Boundaries = QList<LonLat>;
    
    /**
     * Creates an empty FIR.
     */
    Fir();
    
    /**
     * Creates a new Fir with the given ICAO code.
     */
    explicit Fir(const QString& icao);
    
    /**
     * Creates a copy of \c other.
     */
    Fir(const Fir& other);
    
    /**
     * Destroys the Fir.
     */
    virtual ~Fir();
    
    /**
     * Assigns \c other to this Fir.
     */
    Fir& operator =(const Fir& other);
    
    /**
     * Returns the ICAO code of this FIR.
     */
    QString icao() const;
    
    /**
     * Returns the name of this FIR.
     */
    QString name() const;
    
    /**
     * Sets the name of this FIR to the new value.
     */
    void setName(const QString& name);
    
    /**
     * Returns the country of this FIR.
     */
    QString country() const;
    
    /**
     * Sets the country of this FIR to the new value.
     */
    void setCountry(const QString& country);
    
    /**
     * Specifies whether the FIR is oceanic or not.
     */
    bool isOceanic() const;
    
    /**
     * Sets the FIR's oceanic property to the new value.
     */
    void setOceanic(bool oceanic);
    
    /**
     * Returns label position of the given FIR.
     */
    LonLat labelPosition() const;
    
    /**
     * Sets the label position to the given value.
     */
    void setLabelPosition(const LonLat& labelPosition);
    
    /**
     * List of boundaries.
     * Usually, the FIR consists of only one polygon, but in case of the
     * cross-IDL FIRs (like NZZO), it may contain more.
     */
    QList<Boundaries> boundaries() const;
    
    /**
     * Sets the FIR boundaries to the given value.
     */
    void setBoundaries(const QList<Boundaries>& boundaries);
    
private:
    QSharedDataPointer<FirData> d;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::Fir)
Q_DECLARE_TYPEINFO(Vatsinator::Core::Fir, Q_MOVABLE_TYPE);

#endif // CORE_FIR_H
