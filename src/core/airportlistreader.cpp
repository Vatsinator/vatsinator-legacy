/*
 * airportlistreader.cpp
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

#include "airportlistreader.h"
#include <spatial/point_multimap.hpp>
#include <spatial/neighbor_iterator.hpp>
#include <QtCore>

namespace Vatsinator { namespace Core {

class AirportListReaderPrivate {
    
    struct LonLatPointAccessor {
        qreal operator() (spatial::dimension_type dimension, const LonLat& p) const
        {
            switch (dimension) {
                case 0: return p.x();
                case 1: return p.y();
                default: Q_UNREACHABLE();
            }
        }
    };
    
    using ItemMap = spatial::point_multimap<2, LonLat, Airport, spatial::accessor_less<LonLatPointAccessor, LonLat>>;
    
public:
    Airport nearest(const LonLat& p) const
    {
        auto it = spatial::neighbor_cbegin(m_airports, p);
        return it == m_airports.end() ? Airport() : it->second;
    }
    
    Airport findByIcao(const QString& icao) const
    {
        auto it = std::find_if(m_airports.begin(), m_airports.end(), [&icao](auto it) {
           return it.second.icao() == icao; 
        });
        
        return it == m_airports.end() ? Airport() : it->second;
    }
    
    bool isEmpty() const
    {
        return m_airports.size() == 0;
    }
    
    int size() const
    {
        return static_cast<int>(m_airports.size());
    }
    
    void clear()
    {
        m_airports.clear();
    }
    
    void fromJson(const QJsonArray& json)
    {
        for (auto it: json) {
            QJsonObject o = it.toObject();
            Airport a(o.value("icao").toString());
            a.setIata(o.value("iata").toString());
            a.setCity(o.value("city").toString());
            a.setCountry(o.value("country").toString());
            a.setName(o.value("name").toString());
            qreal lon = o.value("longitude").toDouble();
            qreal lat = o.value("latitude").toDouble();
            LonLat position(lon, lat);
            a.setPosition(position);
            a.setAltitude(o.value("altitude").toInt());
            
            m_airports.insert(std::make_pair(a.position(), a));
        }
    }
    
    QJsonArray toJson() const
    {
        QJsonArray array;
        
        for (auto it: m_airports) {
            QJsonObject object;
            object.insert(QStringLiteral("icao"), it.second.icao());
            object.insert(QStringLiteral("iata"), it.second.iata());
            object.insert(QStringLiteral("city"), it.second.city());
            object.insert(QStringLiteral("country"), it.second.country());
            object.insert(QStringLiteral("name"), it.second.name());
            object.insert(QStringLiteral("longitude"), it.second.position().longitude());
            object.insert(QStringLiteral("latitude"), it.second.position().latitude());
            object.insert(QStringLiteral("altitude"), it.second.altitude());
            
            array.append(object);
        }
        
        return array;
    }
    
private:
    ItemMap m_airports;
};


AirportListReader::AirportListReader(QObject* parent) :
    ResourceFile(QStringLiteral("data/airports.json"), parent),
    d(new AirportListReaderPrivate)
{
    readData();
    connect(this, &FileReader::fileRead, this, &AirportListReader::readData);
}

AirportListReader::~AirportListReader() {}

Airport AirportListReader::nearest(const LonLat& point) const
{
    return d->nearest(point);
}

Airport AirportListReader::findByIcao(const QString& icao) const
{
    return d->findByIcao(icao);
}

bool AirportListReader::isEmpty() const
{
    return d->isEmpty();
}

void AirportListReader::readData()
{
    QJsonDocument document = QJsonDocument::fromJson(data());
    Q_ASSERT(document.isArray());
    QJsonArray array = document.array();
    d->clear();
    d->fromJson(array);
    
    qDebug("AirportListReader: read %i airports", d->size());
}

}} /* namespace Vatsinator::Core */
