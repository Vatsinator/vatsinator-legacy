/*
    airport.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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


#ifndef CORE_AIRPORT_H
#define CORE_AIRPORT_H

#include "core/clientlist.h"
#include "core/lonlat.h"
#include "core/v8rcore_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

class Pilot;

/**
 * \ingroup Core
 * @{
 * 
 * The Airport class represents a single airport.
 * 
 * The airport contains basic information, such as ICAO code, position,
 * name, city and country it belongs to.
 */
class V8RCORE_EXPORT Airport : public ClientList {
    Q_OBJECT

    Q_PROPERTY(QString icao READ icao WRITE setIcao NOTIFY icaoChanged)
    Q_PROPERTY(QString iata READ iata WRITE setIata NOTIFY iataChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Vatsinator::Core::LonLat position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged)

signals:
    void icaoChanged(QString icao);
    void iataChanged(QString iata);
    void cityChanged(QString city);
    void countryChanged(QString country);
    void nameChanged(QString name);
    void positionChanged(LonLat position);
    void altitudeChanged(int altitude);

public:
    explicit Airport(const QString& icao, QObject* parent = nullptr);

    const QString& icao() const { return m_icao; }
    void setIcao(const QString& icao);

    const QString& iata() const { return m_iata; }
    void setIata(const QString& iata);

    const QString& city() const { return m_city; }
    void setCity(const QString& city);

    const QString& country() const { return m_country; }
    void setCountry(const QString& country);

    const QString& name() const { return m_name; }
    void setName(const QString& name);

    const LonLat& position() const { return m_position; }
    void setPosition(const LonLat& position);

    int altitude() const { return m_altitude; }
    void setAltitude(int altitude);

    QList<const Pilot*> outboundFlights() const;
    int outboundFlightCount() const;

    QList<const Pilot*> inboundFlights() const;
    int inboundFlightCount() const;

    Q_INVOKABLE QString representativeName() const;

private:
    QString m_icao;
    QString m_iata;
    QString m_city;
    QString m_country;
    QString m_name;
    LonLat m_position;
    int m_altitude;
    
}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_AIRPORT_H
