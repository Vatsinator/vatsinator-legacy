/*
    pilot.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef PILOT_H
#define PILOT_H

#include <QPointF>
#include <QStringList>
#include <QString>
#include <QVector>

#include "vatsimdata/client.h"

class Airport;
struct Point;

/**
 * The Pilot class contains info about one particular pilot - his flight plan,
 * actual position, plane, etc etc. Note that in Vatsinator we use both
 * "Pilot" or "Flight" for the same class. This really needs to be fixed
 * and we should stay consistent in our name conventions. But as for now,
 * don't be surprised when you meet "Flight" anywhere in the code. It is
 * about this exact class.
 */
class Pilot : public Client {
    Q_OBJECT
    Q_ENUMS(FlightRules)
    Q_ENUMS(Phase)
    
    /**
     * This property keeps the Estimated Time of Arrival.
     */
    Q_PROPERTY(QTime eta READ eta NOTIFY progressChanged)
    
    /**
     * This property holds the flight progress [1-100].
     */
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    
    /**
     * This property keeps current altitude.
     */
    Q_PROPERTY(int altitude READ altitude NOTIFY altitudeChanged)
    
    /**
     * This property keeps client's ground speed, expressed in knots.
     */
    Q_PROPERTY(int groundSpeed READ groundSpeed NOTIFY groundSpeedChanged)
    
    /**
     * The client's squawk code.
     * It's string, not int, as squawk might start with '0'.
     */
    Q_PROPERTY(QString squawk READ squawk NOTIFY squawkChanged)
    
    /**
     * This property keeps the client's aircraft.
     */
    Q_PROPERTY(QString aircraft READ aircraft NOTIFY aircraftChanged)
    
    /**
     * The client's current True Air Speed, in knots.
     */
    Q_PROPERTY(int tas READ tas NOTIFY tasChanged)
    
    /**
     * The client's remarks.
     */
    Q_PROPERTY(QString remarks READ remarks NOTIFY remarksChanged)
    
    /**
     * Scheduled Time of Departure.
     */
    Q_PROPERTY(QTime std READ std NOTIFY stdChanged)
    
    /**
     * Actual Time of Departure.
     */
    Q_PROPERTY(QTime atd READ atd NOTIFY atdChanged)
    
    /**
     * Scheduled Time of Arrival.
     */
    Q_PROPERTY(QTime sta READ sta NOTIFY staChanged)
    
    /**
     * This property holds the client's current heading.
     */
    Q_PROPERTY(int heading READ heading NOTIFY headingChanged)
    
    /**
     * The client's flight rules - instrumental or visual.
     */
    Q_PROPERTY(FlightRules flightRules READ flightRules NOTIFY flightRulesChanged)
    
    /**
     * The client's current flight phase.
     * Flight status is determined by the current altitude, speed and distance
     * from airport (origin or destination).
     *
     * \todo Climbing & descending status options.
     */
    Q_PROPERTY(Phase phase READ phase NOTIFY phaseChanged)
    
    /**
     * Prefiled only means that client has prefiled the flight plan, but
     * he did not log in yet.
     *
     * \todo Handle prefiled flights properly.
     */
    Q_PROPERTY(bool prefiledOnly READ isPrefiledOnly NOTIFY wentOnline)
    
    /**
     * Origin airport.
     */
    Q_PROPERTY(const Airport* origin READ origin NOTIFY airportsUpdated)
    
    /**
     * Destination airport.
     */
    Q_PROPERTY(const Airport* destination READ destination NOTIFY airportsUpdated)
    
public:
    /**
     * The FlightRUles enum describes the rules the flight is operatig on.
     * \sa flightRules().
     */
    enum FlightRules {
        Ifr, /**< Instrumental Flight Rules */
        Vfr /**< Visual Flight Rules */
    };
    
    /**
     * The Phase enum describes the phase of the flight.
     * \sa phase().
     */
    enum Phase {
        Departing, /**< The aircraft is still at the origin airport. */
        Airborne, /**< The flight is in progress. */
        Arrived /**< The aircraft is already at the destination airport. */
    };
    
    struct Pressure {
        QString ihg; /**< Inch of mercury */
        QString mb; /**< A.k.a. hectopascal (hPa) - millibar */
    };
    
    /**
     * Route, as filed in the flight plan.
     * To get departure & destination airport pointers directly,
     * see origin() and destination() methods.
     */
    struct Route {
        QString origin;
        QString destination;
        QString route;
        QString altitude;
        
        /* This is vector of points that belong to the route.
         * The first point is the origin airport, the middle one is
         * the pilot position and the last one is the destination airport.
         */
        QVector<LonLat> waypoints;
    };
    
signals:
    void progressChanged();
    void altitudeChanged(int altitude);
    void groundSpeedChanged(int groundSpeed);
    void squawkChanged(const QString& squawk);
    void aircraftChanged(const QString& aircraft);
    void tasChanged(int tas);
    void remarksChanged(const QString& remarks);
    void stdChanged(const QTime& std);
    void atdChanged(const QTime& atd);
    void staChanged(const QTime& sta);
    void headingChanged(int heading);
    void flightRulesChanged(FlightRules flightRules);
    void phaseChanged(Phase phase);
    void wentOnline();
    void airportsUpdated();
    
public:
    /**
     * This constructor instantiates new Pilot from the given _data_.
     * \param data The data line.
     * \param prefiled Indicates whether the flight is only prefiled.
     */
    Pilot(const QStringList& data, bool prefiled = false);
    
    /**
     * \copydoc Client::update()
     */
    void update(const QStringList& data) override;
    
    /**
     * The position of the aircraft from previous update.
     * If this is the first update, current position is kept.
     */
    inline const LonLat& oldPosition() const
    {
        return __oldPosition;
    }
    
    /**
     * The client's baro setting.
     */
    inline const Pilot::Pressure& pressure() const
    {
        return __pressure;
    }
    
    /**
     * The client's route details. It includes planned origin and destination
     * airport, altitude and filled route.
     */
    inline const Pilot::Route& route() const
    {
        return __route;
    }
    
    const QTime& eta() const;
    int progress() const;
    
    inline int altitude() const
    {
        return __altitude;
    }
    
    inline int groundSpeed() const
    {
        return __groundSpeed;
    }
    
    inline const QString& squawk() const
    {
        return __squawk;
    }
    
    inline const QString& aircraft() const
    {
        return __aircraft;
    }
    
    inline int tas() const
    {
        return __tas;
    }
    
    inline const QString& remarks() const
    {
        return __remarks;
    }
    
    inline const QTime& std() const
    {
        return __std;
    }
    
    inline const QTime& atd() const
    {
        return __atd;
    }
    
    inline const QTime& sta() const
    {
        return __sta;
    }
    
    inline unsigned heading() const
    {
        return __heading;
    }
    
    inline Pilot::FlightRules flightRules() const
    {
        return __flightRules;
    }
    
    inline Pilot::Phase phase() const
    {
        return __phase;
    }
    
    inline bool isPrefiledOnly() const
    {
        return __prefiledOnly;
    }
    
    inline const Airport* origin() const
    {
        return __origin;
    }
    
    inline const Airport* destination() const
    {
        return __destination;
    }
    
    Pilot() = delete;
    virtual ~Pilot() = default;
    
private:
    /**
     * Adds origin/destination airports to the vectors.
     */
    void __updateAirports();
    
    /**
     * Finds origin airport, based on the flight plan.
     */
    void __findOrigin();
    
    /**
     * Finds destination airport.
     */
    void __findDestination();
    
    /**
     * Checks whether pilot is departing, airborn or has just arrived.
     */
    Phase __discoverFlightPhase();
    
    /**
     * Checks whether the route crosses the IDL and fixes it.
     */
    void __fixupRoute();
    
    /**
     * Checks (without knowing the flight phase yet) whether the flight
     * is airborne or not.
     */
    bool __maybeAirborne();
    
private slots:
    void __updateWaypoints();
    void __invalidateProgress();
    
private:
    int                   __altitude;
    int                   __groundSpeed;
    QString               __squawk;
    QString               __aircraft;
    LonLat                __oldPosition;
    int                   __tas;
    Pilot::FlightRules    __flightRules;
    QTime                 __std; /* Scheduled Time of Departure  */
    QTime                 __atd; /* Actual Time of Departure */
    QTime                 __sta; /* Scheduled Time of Arrival */
    mutable QTime         __eta; /* Estimated Time of Arrival */
    mutable int           __progress; /* [1-100] */
    QString               __remarks;
    unsigned              __heading;
    Pilot::Phase          __phase;
    Pilot::Pressure       __pressure;
    Pilot::Route          __route;
    const Airport*        __origin;
    const Airport*        __destination;
    bool                  __prefiledOnly;
    
};

Q_DECLARE_METATYPE(const Pilot*)

#endif // PILOT_H
