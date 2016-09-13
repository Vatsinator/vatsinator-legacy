/*
 * tmalistreader.cpp
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

#include "tmalistreader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

TmaListReader::TmaListReader(QObject* parent) :
    ResourceFile(QStringLiteral("data/tma.json"), parent)
{
    readData();
    connect(this, &FileReader::fileRead, this, &TmaListReader::readData);
}

QList<LonLat> TmaListReader::coords(const QString& icao) const
{
    return m_coords.value(icao);
}

void TmaListReader::addTma(const QString& icao, const QJsonArray& array)
{
    qreal y = qQNaN();
    QList<LonLat> points;
    
    for (const auto& value: array) {
        Q_ASSERT(value.isDouble());
        
        if (qIsNaN(y))
            y = value.toDouble();
        else {
            double x = value.toDouble();
            points.append(LonLat(x, y));
            y = qQNaN();
        }
    }
    
    Q_ASSERT(qIsNaN(y));
    
    m_coords.insert(icao, points);
}

void TmaListReader::readData()
{
    QJsonDocument document = QJsonDocument::fromJson(data());
    Q_ASSERT(document.isObject());
    QJsonObject object = document.object();
    auto keys = object.keys();
    for (auto key: keys) {
        auto value = object.value(key);
        Q_ASSERT(value.isArray());;
        auto array = value.toArray();
        addTma(key, array);
    }
    
    qDebug("TmaListReader: read %d TMAs", m_coords.size());
}

}} /* namespace Vatsinator::Core */
