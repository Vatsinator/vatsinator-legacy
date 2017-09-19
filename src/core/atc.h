/*
 * atc.h
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

#ifndef CORE_ATC_H
#define CORE_ATC_H

#include "core/airport.h"
#include "core/client.h"
#include "core/fir.h"
#include "core/v8rcore_export.h"
#include <QtCore/QString>

namespace Vatsinator { namespace Core {

class ClientData;
class AtcTracker;

/**
 * \ingroup Core
 * @{
 *
 * The Atc class represents a single ATC.
 */
class V8RCORE_EXPORT Atc : public Client {
    Q_OBJECT
    Q_ENUMS(Facility)
    Q_ENUMS(Rating)

    friend class ClientData; // for icao field
    friend class AtcTracker; // for icao field
    
    /**
     * Stores ATC rating.
     */
    Q_PROPERTY(Rating rating READ rating WRITE setRating NOTIFY ratingChanged)
    
    /**
     * The frequency the ATC operates on.
     */
    Q_PROPERTY(QString frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    
    /**
     * The current ATIS message.
     */
    Q_PROPERTY(QString atis READ atis WRITE setAtis NOTIFY atisChanged)
    
    /**
     * Stores facility of the ATC.
     */
    Q_PROPERTY(Facility facility READ facility WRITE setFacility NOTIFY facilityChanged)
    
    /**
     * Auto-generated ATC description (e.g. "Stansted Tower").
     */
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    
    /**
     * The airport this ATC is at. If the ATC is not bound to any airport (i.e.
     * its \c facility is \c Ctr or \c Fss), value of this property is \c nullptr.
     */
    Q_PROPERTY(Vatsinator::Core::Airport* airport READ airport WRITE setAirport NOTIFY airportChanged)
    
    /**
     * The FIR this ATC is bound to. Value of this property is not \c nullptr only
     * if the \c facility is \c Ctr or \c Fss.
     */
    Q_PROPERTY(Vatsinator::Core::Fir* fir READ fir WRITE setFir NOTIFY firChanged)

    /**
     * Specifies whether this ATC controls UIR (Upper FIR).
     */
    Q_PROPERTY(bool uir READ isUir WRITE setIsUir)

public:
    /**
     * Describes the ATC position.
     */
    enum Facility {
        Atis = 0x1  /**< ATIS */,
        Del = 0x2   /**< Delivery */,
        Gnd = 0x4   /**< Ground */,
        Twr = 0x8   /**< Tower */,
        App = 0x10  /**< Approach */,
        Dep = 0x20  /**< Departure */,
        Ctr = 0x40  /**< Control */,
        Fss = 0x80  /**< Control */,
        Obs = 0x100 /**< Observer */
    };
    Q_DECLARE_FLAGS(Facilities, Facility)
    
    /**
     * Represents VATSIM Controller global ratings.
     * \sa https://www.vatsim.net/documents/global-ratings-policy
     */
    enum Rating {
        Observer = 1 /**< Observer */,
        TowerTrainee = 2 /**< Tower Trainee (S1) */,
        TowerController = 3 /**< Tower Controller (S2) */,
        TmaController = 4 /**< TMA Controller (S3) */,
        EnrouteController = 5 /**< Enroute Controller (C1) */,
        SeniorController = 7 /**< Senior Controller (C3) */
    };
    
signals:
    void ratingChanged(Rating rating);
    void frequencyChanged(const QString& frequency);
    void atisChanged(const QString& atis);
    void facilityChanged(Facility facility);
    void descriptionChanged(const QString& description);
    void airportChanged(Airport* airport);
    void firChanged(Fir* fir);

public:
    explicit Atc(quint32 pid, ServerTracker* server);
    virtual ~Atc();
    
    Rating rating() const { return m_rating; }
    void setRating(Rating rating);
    const QString& frequency() const { return m_frequency; }
    void setFrequency(const QString& frequency);
    const QString& atis() const { return m_atis; }
    void setAtis(const QString& atis);
    Facility facility() const { return m_facility; }
    void setFacility(Facility facility);
    const QString& description() const { return m_description; }
    void setDescription(const QString& description);
    Airport* airport() const { return m_airport; }
    void setAirport(Airport* airport);
    Fir* fir() const { return m_fir; }
    void setFir(Fir* fir);
    bool isUir() const { return m_uir; }
    void setIsUir(bool isUir);
    
    /**
     * Converts the provided \c facility to string.
     */
    static QString facilityStr(Facility facility);
    
    /**
     * Converts the given \c rating to string.
     */
    static QString ratingStr(Rating rating);
    
protected:
    QString icao;
    
private:
    static QString cleanupAtis(const QString& orig);
    
private slots:
    void updateDescription();
    
private:
    Rating m_rating;
    QString m_frequency;
    QString m_atis;
    Facility m_facility;
    QString m_description;
    Airport* m_airport = nullptr;
    Fir* m_fir = nullptr;
    bool m_uir = false;

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(const Vatsinator::Core::Atc*)
Q_DECLARE_OPERATORS_FOR_FLAGS(Vatsinator::Core::Atc::Facilities)

#endif // CORE_ATC_H
