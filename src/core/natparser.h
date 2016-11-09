/*
 * natparser.h
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

#ifndef CORE_NATPARSER_H
#define CORE_NATPARSER_H

#include "coreexport.h"
#include "core/lonlat.h"
#include <QString>
#include <QList>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * The NatParser class is responsible for finding a NAT (North Atlantic
 * Route) in the route filled by a pilot.
 */
class __VtrCoreApi__ NatParser {
public:
    /**
     * Creates the new \c NatParser instance that will try to find NAT
     * in the \c filledRoute.
     */
    NatParser(const QString& filledRoute);

    /**
     * Destroys this \c NatParser instance.
     */
    virtual ~NatParser();

    /**
     * Tries to find NAT using different methods. Each method looks for
     * another type of filling NATs in the flight plan.
     */
    virtual bool natFound();

    /**
     * Returns list of parsed waypoints. If the NAT could not be found,
     * returns an empty list.
     *
     * \note Access waypoints only after calling \ref natFound().
     */
    const QList<LonLat> waypoints() const { return m_waypoints; }

protected:
    bool natMethod1();
    bool natMethod2();
    bool natMethod3();
    bool natMethod4();

private:
    QString m_filledRoute;
    QList<LonLat> m_waypoints;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_NATPARSER_H
