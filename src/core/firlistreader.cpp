/*
 * firlistreader.cpp
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

#include "firlistreader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

FirListReader::FirListReader(QObject* parent) :
    ResourceFile("data/firs.json", parent)
{
    readData();
    connect(this, &FileReader::fileRead, this, &FirListReader::readData);
}

void FirListReader::readData()
{
    QJsonDocument document = QJsonDocument::fromJson(data());
    
    QJsonArray array = document.array();
    m_firs.clear();
    
    for (auto it: array) {
        QJsonObject o = it.toObject();
        
        Fir f(o.value("icao").toString());
        f.setName(o.value("name").toString());
        f.setCountry(o.value("country").toString());
        Q_ASSERT(o.value("oceanic").isBool());
        f.setOceanic(o.value("oceanic").toBool());
        
        auto labelPosition = o.value("labelPosition").toObject();
        f.setLabelPosition(LonLat::fromJson(labelPosition));
        
        QList<Fir::Boundaries> boundaries;
        auto jBoundaries = o.value("border");
        if (jBoundaries.isArray() && jBoundaries.toArray().first().isArray()) {
            // list of boundaries (i.e. array of arrays of coords)
            for (const auto& border: jBoundaries.toArray()) {
                QList<LonLat> b;
                for (auto p: border.toArray())
                    b << LonLat::fromJson(p.toObject());
                
                boundaries << b;
            }
        } else if (jBoundaries.isArray() && jBoundaries.toArray().first().isObject()) {
            // boundaries (i.e. array of coords)
            QList<LonLat> b;
            for (auto p: jBoundaries.toArray()) {
                b << LonLat::fromJson(p.toObject());
            }
            
            boundaries << b;
        }
        
        f.setBoundaries(boundaries);
        
        m_firs.append(f);
    }
    
    qDebug("FirListReader: read %i firs", m_firs.size());
}

}} /* namespace Vatsinator::Core */
