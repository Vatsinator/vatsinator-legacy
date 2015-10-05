/*
    airport.cpp
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

#include <algorithm>
#include <QtCore>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "models/atctablemodel.h"
#include "models/flighttablemodel.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "airport.h"

Airport::Airport(const AirportRecord* record, QObject* parent) :
    QObject(parent),
    __data(record),
    __icao(__data->icao),
    __iata(__data->iata),
    __city(QString::fromUtf8(__data->city)),
    __country(QString::fromUtf8(__data->country)),
    __name(QString::fromUtf8(__data->name)),
    __altitude(__data->altitude),
    __staff(new AtcTableModel(this)),
    __inbounds(new FlightTableModel(this)),
    __outbounds(new FlightTableModel(this))
{
    Q_ASSERT(__data);
    Q_ASSERT(VatsimDataHandler::isValidIcao(__icao));
    
    auto fillFir = [this](const QString& icao, bool isFss) {
        Fir* f = vApp()->vatsimDataHandler()->findFir(icao, isFss);
        
        if (f) {
            f->addAirport(this);
            __firs << f;
        }
    };
    
    fillFir(QString(__data->fir_a), __data->is_fir_a_oceanic);
    fillFir(QString(__data->fir_b), __data->is_fir_b_oceanic);
}

Airport::Airport(const QString& icao, QObject* parent) : 
    QObject(parent),
    __data(nullptr),
    __icao(icao),
    __altitude(0),
    __staff(nullptr),
    __inbounds(nullptr),
    __outbounds(nullptr)
{
    
}

Airport::Airport(QObject* parent) :
    QObject (parent),
    __data(nullptr),
    __altitude(0),
    __staff(nullptr),
    __inbounds(nullptr),
    __outbounds(nullptr)
{

}

unsigned
Airport::countDepartures(bool includePrefiled) const
{
    if (isValid()) {
        return std::count_if(__outbounds->toList().begin(), __outbounds->toList().end(), [includePrefiled](const Pilot * p) {
            return p->phase() == Pilot::Departing && (!p->isPrefiledOnly() || (p->isPrefiledOnly() && includePrefiled));
        });
    } else {
        return 0;
    }
}

unsigned
Airport::countOutbounds() const
{
    if (isValid())
        return __outbounds->rowCount();
    else
        return 0;
}

unsigned
Airport::countArrivals() const
{
    if (isValid()) {
        return std::count_if(__inbounds->toList().begin(), __inbounds->toList().end(), [](const Pilot * p) {
                return p->phase() == Pilot::Arrived;
        });
    } else {
        return 0;
    }
}

unsigned
Airport::countInbounds() const
{
    if (isValid())
        return __inbounds->rowCount();
    else
        return 0;
}

Controller::Facilities
Airport::facilities() const
{
    if (!isValid())
        return 0;
    
    Controller::Facilities facilities = 0;
    
    for (const Controller* c : __staff->toList())
        facilities |= c->facility();
        
    return facilities;
}

void
Airport::addStaff(const Controller* atc)
{
    Q_ASSERT(isValid());
    __staff->add(atc);
    connect(atc, &Controller::updated, this, &Airport::updated);
    connect(atc, &Controller::destroyed, this, &Airport::updated, Qt::DirectConnection);
    emit updated();
}

void
Airport::addInbound(const Pilot* pilot)
{
    Q_ASSERT(isValid());
    __inbounds->add(pilot);
    connect(pilot, &Pilot::updated, this, &Airport::updated);
    connect(pilot, &Pilot::destroyed, this, &Airport::updated, Qt::DirectConnection);
            
    for (Fir* f : __firs)
        f->addFlight(pilot);
        
    emit updated();
}

void
Airport::addOutbound(const Pilot* pilot)
{
    Q_ASSERT(isValid());
    __outbounds->add(pilot);
    connect(pilot, &Pilot::updated, this, &Airport::updated);
    connect(pilot, &Pilot::destroyed, this, &Airport::updated, Qt::DirectConnection);
            
    for (Fir* f : __firs)
        f->addFlight(pilot);
        
    emit updated();
}

bool
Airport::isEmpty() const
{
    if (isValid())
        return __staff->rowCount() == 0 && __inbounds->rowCount() == 0 && __outbounds->rowCount() == 0;
    else
        return true;
}

bool
Airport::isStaffed() const
{
    if (isValid())
        return __staff->rowCount() > 0 || __inbounds->rowCount() > 0 || __outbounds->rowCount() > 0;
    else
        return false;
}

LonLat
Airport::position() const
{
    if (isValid())
        return LonLat(__data->longitude, __data->latitude);
    else
        return LonLat();
}
