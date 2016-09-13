/*
 * airlinelistreader.h
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

#ifndef CORE_AIRLINELISTREADER_H
#define CORE_AIRLINELISTREADER_H

#include "airline.h"
#include "coreexport.h"
#include "resourcefile.h"
#include <QMap>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 */
class __VtrCoreApi__ AirlineListReader : public ResourceFile {
    Q_OBJECT
    
public:
    explicit AirlineListReader(QObject* parent = nullptr);
    
    virtual ~AirlineListReader();
    
    Airline findByIcao(const QString& icao) const;
    
private slots:
    void readData();
    
private:
    QMap<QString, Airline> m_airlines; /**< ICAO <-> airline pairs */


}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_AIRLINELISTREADER_H
