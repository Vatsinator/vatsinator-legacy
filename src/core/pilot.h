/*
 * pilot.h
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#ifndef CORE_PILOT_H
#define CORE_PILOT_H

#include "airline.h"
#include "airportobject.h"
#include "client.h"
#include "coreexport.h"
#include "flightplan.h"
#include <QMetaType>
#include <QString>

namespace Vatsinator { namespace Core {

class ClientData;
class ServerTracker;

/**
 * \ingroup Core
 * @{
 *
 * The Pilot class represents a single flight.
 */
class __VtrCoreApi__ Pilot : public Client {
    Q_OBJECT
    Q_ENUMS(FlightRules)
    Q_ENUMS(FlightPhase)
    
    class RouteParserTask;
    
    /**
     * Estimated time of arrival.
     */
    Q_PROPERTY(QTime eta READ eta WRITE setEta NOTIFY etaChanged)
    
    /**
     * Flight progress [0-100].
     */
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)
    
    /**
     * Current altitude.
     */
    Q_PROPERTY(int altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged)
    
    /**
     * Current ground speed (in knots).
     */
    Q_PROPERTY(int groundSpeed READ groundSpeed WRITE setGroundSpeed NOTIFY groundSpeedChanged)
    
    /**
     * Squawk.
     */
    Q_PROPERTY(QString squawk READ squawk WRITE setSquawk NOTIFY squawkChanged)
    
    /**
     * Aircraft model (as filled in the flight plan).
     */
    Q_PROPERTY(QString aircraft READ aircraft WRITE setAircraft NOTIFY aircraftChanged)
    
    /**
     * Current True Air Speed.
     */
    Q_PROPERTY(int tas READ tas WRITE setTas NOTIFY tasChanged)
    
    /**
     * Flight plan remarks.
     */
    Q_PROPERTY(QString remarks READ remarks WRITE setRemarks NOTIFY remarksChanged)
    
    /**
     * Standard time of departure.
     */
    Q_PROPERTY(QTime std READ std WRITE setStd NOTIFY stdChanged)
    
    /**
     * Scheduled time of arrival.
     */
    Q_PROPERTY(QTime sta READ sta WRITE setSta NOTIFY staChanged)
    
    /**
     * Actual time of departure.
     */
    Q_PROPERTY(QTime atd READ atd WRITE setAtd NOTIFY atdChanged)
    
    /**
     * Current heading.
     */
    Q_PROPERTY(int heading READ heading WRITE setHeading NOTIFY headingChanged)
    
    /**
     * Flight rules, as filled in the flight plan.
     */
    Q_PROPERTY(FlightRules flightRules READ flightRules WRITE setFlightRules NOTIFY flightRulesChanged)
    
    /**
     * Current flight phase.
     */
    Q_PROPERTY(FlightPhase flightPhase READ flightPhase WRITE setFlightPhase NOTIFY flightPhaseChanged)
    
    /**
     * Departure airport.
     */
    Q_PROPERTY(AirportObject* departure READ departure WRITE setDeparture NOTIFY departureChanged)
    
    /**
     * Destination airport.
     */
    Q_PROPERTY(AirportObject* destination READ destination WRITE setDestination NOTIFY destinationChanged)
    
    /**
     * Cruise altitude, as filled in the flight plan.
     */
    Q_PROPERTY(QString cruiseAltitude READ cruiseAltitude WRITE setCruiseAltitude NOTIFY cruiseAltitudeChanged)
    
    /**
     * Planned True Air Speed.
     */
    Q_PROPERTY(int plannedTas READ plannedTas WRITE setPlannedTas NOTIFY plannedTasChanged)

    /**
     * The filled flight plan.
     */
    Q_PROPERTY(FlightPlan flightPlan READ flightPlan WRITE setFlightPlan NOTIFY flightPlanChanged)
    
    /**
     * The airline that is operating this flight.
     */
    Q_PROPERTY(Airline airline READ airline WRITE setAirline NOTIFY airlineChanged)
    
public:
    /**
     * The \c FlightRules enum describes the rules the flight is operatig on.
     * \sa flightRules().
     */
    enum FlightRules {
        Ifr /**< Instrumental Flight Rules */,
        Vfr /**< Visual Flight Rules */
    };
    
    /**
     * The \c FlightPhase enum describes the phase of the flight.
     * \sa flightPhase().
     */
    enum FlightPhase {
        Departing /**< The aircraft is still at the origin airport. */,
        Airborne /**< The flight is in progress. */,
        Arrived /**< The aircraft is already at the destination airport. */
    };
    
    /**
     * The \c NodeSelectionFlag is used to specify which part of route
     * is to be fetched in the \ref nodes() method.
     */
    enum NodeSelectionFlag {
        DepartureToPilot    = 0x01, /**< First node is departure airport,
                                         last is the pilot's position. */
        PilotToDestination  = 0x02 /**< First node is the pilot's position,
                                         last is the destination airport. */
    };
    Q_DECLARE_FLAGS(NodeSelection, NodeSelectionFlag)
    
signals:
    void etaChanged();
    void progressChanged();
    void altitudeChanged(int altitude);
    void groundSpeedChanged(int groundSpeed);
    void squawkChanged(const QString& squawkChanged);
    void aircraftChanged(const QString& aircraft);
    void tasChanged(int tas);
    void remarksChanged(const QString& remarks);
    void stdChanged(const QTime& std);
    void staChanged(const QTime& sta);
    void atdChanged(const QTime& atd);
    void headingChanged(int heading);
    void flightRulesChanged(FlightRules flightRules);
    void flightPhaseChanged(FlightPhase flightPhase);
    void departureChanged(AirportObject* departure);
    void destinationChanged(AirportObject* destination);
    void cruiseAltitudeChanged(const QString& cruiseAltitude);
    void plannedTasChanged(int plannedTas);
    void flightPlanChanged(const FlightPlan& flightPlan);
    void airlineChanged(const Airline& airline);

    /**
     * Emitted when the nodes on the route are updated, i.e. the
     * route is parsed.
     */
    void nodesUpdated();

public:
    /**
     * Creates a new Pilot instance.
     */
    explicit Pilot(quint32 pid, ServerTracker* server);
    
    /**
     * Destroys the Pilot instance.
     */
    virtual ~Pilot();
    
    /**
     * Returns list of points that make the flight route.
     *
     * \note With the first call to this function only the nodes between airports
     * and the pilot will be returned. The route will be parsed on a separate thread.
     * When the full list of nodes is ready, the \c nodesUpdated() signal will be
     * emitted.
     */
    QList<LonLat> nodes(NodeSelection selection = NodeSelection(DepartureToPilot | PilotToDestination)) const;
    
    const QTime& eta() const { if (!m_etaCalculated) calculateEta(); return m_eta; }
    void setEta(const QTime& eta);
    int progress() const { if (m_progress == -1) calculateProgress(); return m_progress; }
    void setProgress(int progress);
    int altitude() const { return m_altitude; }
    void setAltitude(int altitude);
    int groundSpeed() const { return m_groundSpeed; }
    void setGroundSpeed(int groundSpeed);
    const QString& squawk() const { return m_squawk; }
    void setSquawk(const QString& squawk);
    const QString& aircraft() const { return m_aircraft; }
    void setAircraft(const QString& aircraft);
    int tas() const { return m_tas; }
    void setTas(int tas);
    const QString& remarks() const { return m_remarks; }
    void setRemarks(const QString& remarks);
    const QTime& std() const { return m_std; }
    void setStd(const QTime& std);
    const QTime& sta() const { return m_sta; }
    void setSta(const QTime& sta);
    const QTime& atd() const { return m_atd; }
    void setAtd(const QTime& atd);
    int heading() const { return m_heading; }
    void setHeading(int heading);
    FlightRules flightRules() const { return m_flightRules; }
    void setFlightRules(FlightRules flightRules);
    FlightPhase flightPhase() const { return m_flightPhase; }
    void setFlightPhase(FlightPhase flightPhase);
    AirportObject* departure() const { return m_departure; }
    void setDeparture(AirportObject* departure);
    AirportObject* destination() const { return m_destination; }
    void setDestination(AirportObject* destination);
    const QString& cruiseAltitude() const { return m_cruiseAltitude; }
    void setCruiseAltitude(const QString& cruiseAltitude);
    int plannedTas() const { return m_plannedTas; }
    void setPlannedTas(int plannedTas);
    const FlightPlan& flightPlan() const { return m_flightPlan; }
    void setFlightPlan(FlightPlan flightPlan);
    const Airline& airline() const { return m_airline; }
    void setAirline(const Airline& airline);

    /**
     * Minimum ground speed (in knots) at which the flight is considered
     * airborne.
     */
    static constexpr int MinimumAirborneGroundSpeed() { return 50; }

    /**
     * Maximum distance from the departure/destination airport, in nautical
     * miles.
     */
    static constexpr qreal MaximumDistanceFromAirpoirt() { return 2.0; }

protected slots:
    /**
     * Connected to \ref groundSpeedChanged(), \ref departureChanged() and
     * \ref destinationChanged(); updates the \ref flightPhase property.
     */
    virtual void rediscoverFlightPhase();

    /**
     * Clears route parser status.
     */
    void resetNodes();
    
private:
    void calculateEta() const;
    void calculateProgress() const;
    void setNodes(QList<LonLat> nodes);
    
    mutable QTime m_eta;
    mutable bool m_etaCalculated;
    mutable int m_progress = -1; // -1 means it needs re-calculation
    int m_altitude = 0;
    int m_groundSpeed = 0;
    QString m_squawk;
    QString m_aircraft;
    int m_tas = 0;
    QString m_remarks;
    QTime m_std;
    QTime m_sta;
    QTime m_atd;
    int m_heading = 0;
    FlightRules m_flightRules = Vfr;
    FlightPhase m_flightPhase = Departing;
    AirportObject* m_departure = nullptr;
    AirportObject* m_destination = nullptr;
    QString m_cruiseAltitude;
    int m_plannedTas;
    FlightPlan m_flightPlan;
    Airline m_airline;

    std::tuple<QList<LonLat>, QList<LonLat>> m_nodes;
    mutable int m_routeParserStatus = 0; // 0 - not parsed, 1 - is being parsed, 2 - parsed

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::Pilot*)
Q_DECLARE_METATYPE(const Vatsinator::Core::Pilot*)
Q_DECLARE_METATYPE(Vatsinator::Core::Pilot::FlightRules)
Q_DECLARE_METATYPE(Vatsinator::Core::Pilot::FlightPhase)
Q_DECLARE_OPERATORS_FOR_FLAGS(Vatsinator::Core::Pilot::NodeSelection)

#endif // CORE_PILOT_H
