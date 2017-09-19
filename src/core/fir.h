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

#include "core/clientlist.h"
#include "core/lonlat.h"
#include "core/v8rcore_export.h"

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The \c Fir class represents a single FIR (Flight Information Region).
 */
class V8RCORE_EXPORT Fir : public ClientList {
    Q_OBJECT

    Q_PROPERTY(QString icao READ icao CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(bool oceanic READ isOceanic WRITE setOceanic NOTIFY oceanicChanged)
    Q_PROPERTY(Vatsinator::Core::LonLat labelPosition READ labelPosition WRITE setLabelPosition NOTIFY labelPositionChanged)

public:
    /* List of points that make the boundaries of the FIR. */
    using Boundaries = QList<LonLat>;

signals:
    void nameChanged(QString name);
    void countryChanged(QString country);
    void oceanicChanged(bool oceanic);
    void labelPositionChanged(LonLat labelPosition);
    void boundariesChanged(const QList<Boundaries>& boundaries);

public:
    /**
     * Creates an empty FIR.
     */
    Fir(const QString& icao, QObject* parent = nullptr);
    
    /**
     * Returns the ICAO code of this FIR.
     */
    const QString& icao() const { return m_icao; }
    
    /**
     * Returns the name of this FIR.
     */
    const QString& name() const { return m_name; }
    
    /**
     * Sets the name of this FIR to the new value.
     */
    void setName(const QString& name);
    
    /**
     * Returns the country of this FIR.
     */
    const QString& country() const { return m_country; }
    
    /**
     * Sets the country of this FIR to the new value.
     */
    void setCountry(const QString& country);
    
    /**
     * Specifies whether the FIR is oceanic or not.
     */
    bool isOceanic() const { return m_oceanic; }
    
    /**
     * Sets the FIR's oceanic property to the new value.
     */
    void setOceanic(bool oceanic);
    
    /**
     * Returns label position of the given FIR.
     */
    LonLat labelPosition() const { return m_labelPosition; }
    
    /**
     * Sets the label position to the given value.
     */
    void setLabelPosition(const LonLat& labelPosition);
    
    /**
     * List of boundaries.
     * Usually, the FIR consists of only one polygon, but in case of the
     * cross-IDL FIRs (like NZZO), it may contain more.
     */
    const QList<Boundaries>& boundaries() const { return m_boundaries; }
    
    /**
     * Sets the FIR boundaries to the given value.
     */
    void setBoundaries(const QList<Boundaries>& boundaries);
    
private:
    QString m_icao;
    QString m_name;
    QString m_country;
    bool m_oceanic;
    LonLat m_labelPosition;
    QList<Boundaries> m_boundaries;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_FIR_H
