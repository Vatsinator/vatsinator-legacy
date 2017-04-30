/*
 * tmalistreader.h
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

#ifndef CORE_TMALISTREADER_H
#define CORE_TMALISTREADER_H

#include "core/lonlat.h"
#include "core/resourcefile.h"
#include "core/vtrcore_export.h"
#include <QtCore/QList>
#include <QtCore/QMap>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * A reader for TMA list.
 */
class VTRCORE_EXPORT TmaListReader : public ResourceFile {
    Q_OBJECT
    
public:
    /**
     * Creates a new \c TmaListReader instance, passes \c parent to the QObject.
     */
    explicit TmaListReader(QObject* parent = nullptr);
    
    /**
     * Returns an empty list if no coords for the given TMA were found.
     */
    QList<LonLat> coords(const QString& icao) const;
    
private:
    void addTma(const QString& icao, const QJsonArray& array);

private slots:
    void readData();
    
private:
    using CoordList = QList<LonLat>;
    QMap<QString, CoordList> m_coords;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_TMALISTREADER_H
