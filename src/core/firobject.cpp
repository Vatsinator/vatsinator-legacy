/*
 * firobject.cpp
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

#include "firobject.h"
#include "core/atc.h"
#include "core/lonlat.h"
#include "core/servertracker.h"
#include <QtCore>

namespace {

/**
 * Returns \c true if \c polygon contains \c point.
 */    
bool contains(const QList<Vatsinator::Core::LonLat>& polygon,
              const Vatsinator::Core::LonLat& point)
{
    QPointF topLeft = polygon.first(), bottomRight = polygon.first();
    for (auto p: polygon) {
        if (topLeft.y() < p.y())
            topLeft.setY(p.y());
        
        if (bottomRight.y() > p.y())
            bottomRight.setY(p.y());
        
        if (topLeft.x() > p.x())
            topLeft.setX(p.x());
        
        if (bottomRight.x() < p.x())
            bottomRight.setX(p.x());
    }
    
    QRectF boundingRect = QRectF(topLeft, bottomRight);
    if (!boundingRect.contains(point))
        return false;
    
    bool result = false;
    for (int i = 0; i < polygon.size(); ++i) {
        int j = (i + 1) % polygon.size();
        
        /* point in polygon test */
        if (((polygon.at(j).y() <= point.y() && point.y() < polygon.at(i).y()) || (polygon.at(i).y() <= point.y() && point.y() < polygon.at(j).y())) &&
            (point.x() < polygon.at(j).x() + (polygon.at(i).x() - polygon.at(j).x()) * (point.y() - polygon.at(j).y()) / (polygon.at(i).y() - polygon.at(j).y())))
            result = !result;
    }
    
    return result;
}

}

namespace Vatsinator { namespace Core {

FirObject::FirObject(const Fir& fir, QObject* parent) :
    ClientList(parent),
    m_fir(fir)
{
    if (ServerTracker* t = qobject_cast<ServerTracker*>(parent))
        connect(t, &ServerTracker::airportAdded, this, &FirObject::trackAirport);
}

void FirObject::trackAirport(AirportObject* airport)
{
    if (belongs(airport)) {
        connect(airport, &AirportObject::clientAdded, this, &ClientList::add);
    }
}

bool FirObject::belongs(AirportObject* airport)
{
    /* TODO Store FIRs in airports.json to optimize */
    
    for (auto boundary: boundaries()) {
        if (::contains(boundary, airport->position()))
            return true;
    }
    
    return false;
}

}} /* namespace Vatsinator::Core */

static void registerType()
{
    qRegisterMetaType<Vatsinator::Core::FirObject*>("FirObject*");
}
Q_COREAPP_STARTUP_FUNCTION(registerType)
