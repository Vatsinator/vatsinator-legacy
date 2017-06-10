/*
 * flighttracker.h
 * Copyright (C) 2017  Michał Garapich <michal@garapich.pl>
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

#ifndef FLIGHTTRACKER_H
#define FLIGHTTRACKER_H

#include "core/pilot.h"
#include "core/vtrcore_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 *
 * The FlightTracker class handles flight changes logic.
 */
class VTRCORE_NO_EXPORT FlightTracker : public QObject {
    Q_OBJECT

    /**
     * The \c flight property keeps the tracked pilot.
     */
    Q_PROPERTY(Vatsinator::Core::Pilot* flight READ flight CONSTANT)

public:
    /**
     * Creates a new \c FlightTracker instance that tracks the specified
     * \c flight.
     */
    explicit FlightTracker(Pilot* flight, QObject* parent = nullptr);

    Pilot* flight() { return m_flight; }
    const Pilot* flight() const { return m_flight; }

private:
    std::tuple<Airport*, Airport*> findAirports();

private slots:
    void update();
    void updateAirline();
    void invalidateAirports();

private:
    Pilot* m_flight;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // FLIGHTTRACKER_H
