/*
 * airportlistreader.h
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_AIRPORTLISTREADER_H
#define CORE_AIRPORTLISTREADER_H

#include "airport.h"
#include "coreexport.h"
#include "lonlat.h"
#include "resourcefile.h"
#include <QScopedPointer>

namespace Vatsinator { namespace Core {

class AirportListReaderPrivate;

/**
 * \ingroup Core
 * @{
 *
 * A reader for airport list.
 */
class __VtrCoreApi__ AirportListReader : public ResourceFile {
    Q_OBJECT

public:
    /**
     * Creates a new airport list reader. Passes \c parent to the
     * QObject's constructor.
     */
    explicit AirportListReader(QObject* parent = nullptr);
    
    /**
     * Destroys the reader.
     */
    virtual ~AirportListReader();
    
    /**
     * Finds the airport that is closes to the specified \c point.
     */
    Airport nearest(const LonLat& point) const;
    
    /**
     * Finds the airport based on the given ICAO code. Returns an empty airport
     * if there are no matches.
     */
    Airport findByIcao(const QString& icao) const;
    
    /**
     * Returns \c true if the airport database has no airports.
     */
    bool isEmpty() const;
    
private slots:
    void readData();
    
private:
    QScopedPointer<AirportListReaderPrivate> d;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_AIRPORTLISTREADER_H
