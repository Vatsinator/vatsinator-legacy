/*
 * airlinelistreader.cpp
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

#include "airlinelistreader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

AirlineListReader::AirlineListReader(QObject* parent) :
    ResourceFile(QStringLiteral("data/airlines.json"), parent)
{
    readData();
    connect(this, &FileReader::fileRead, this, &AirlineListReader::readData);
}

AirlineListReader::~AirlineListReader() {}

Airline AirlineListReader::findByIcao(const QString& icao) const
{
    return m_airlines.value(icao);
}

void AirlineListReader::readData()
{
    QJsonDocument document = QJsonDocument::fromJson(data());
    Q_ASSERT(document.isArray());
    QJsonArray array = document.array();
    for (const auto& a: array) {
        Q_ASSERT(a.isObject());
        QJsonObject object = a.toObject();
        Q_ASSERT(object.contains("icao"));
        
        Airline airline;
        airline.setIcao(object.value("icao").toString());
        
        if (object.contains("name"))
            airline.setName(object.value("name").toString());
        
        if (object.contains("iata"))
            airline.setIata(object.value("iata").toString());
        
        if (m_airlines.contains(airline.icao())) {
            qWarning("%s already exists, omitting", qPrintable(airline.icao()));
            continue;
        }
        
        m_airlines.insert(airline.icao(), airline);
    }
    
    qDebug("AirlineListReader: read %i airlines", m_airlines.size());
}


}} /* namespace Vatsinator::Core */
