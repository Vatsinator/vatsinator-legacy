/*
    fir.cpp
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

#include <QtCore>

#include "db/firdatabase.h"
#include "models/airporttablemodel.h"
#include "models/atctablemodel.h"
#include "models/flighttablemodel.h"
#include "vatsimdata/controller.h"

#include "fir.h"

Fir::Fir(const FirRecord* record) :
    __data{ record },
    __icao(QString::fromUtf8(record->header.icao)),
    __oceanic(record->header.oceanic),
    __labelPosition(record->header.textPosition.x, record->header.textPosition.y),
    __staff(new AtcTableModel(this)),
    __uirStaff(new AtcTableModel(this)),
    __flights(new FlightTableModel(this)),
    __airports(new AirportTableModel(this))
{
    Q_ASSERT(record);
}

Fir::~Fir() {}

void
Fir::addStaff(const Controller* atc)
{
    __staff->add(atc);
    connect(atc, &Client::updated, this, &Fir::updated);
    connect(atc, &Client::invalid, this, &Fir::updated);
    emit updated();
}

void
Fir::addUirStaff(const Controller* atc)
{
    __uirStaff->add(atc);
    connect(atc, &Client::updated, this, &Fir::updated);
    connect(atc, &Client::invalid, this, &Fir::updated);
    emit updated();
}

void
Fir::addFlight(const Pilot* pilot)
{
    __flights->add(pilot);
    connect(pilot, &Client::invalid, this, &Fir::updated);
}

void
Fir::addAirport(const Airport* airport)
{
    __airports->addAirport(airport);
}

void
Fir::addRecord(const FirRecord* record)
{
    Q_ASSERT(QString::fromUtf8(record->header.icao) == icao());
    __data << record;
    
    if (__labelPosition.isNull())
        __labelPosition = LonLat(record->header.textPosition.x, record->header.textPosition.y);
}

void
Fir::fixupName()
{
    if (!QRegularExpression("\\b(Radar|Control|Radio|Oceanic)\\b").match(__name).hasMatch())
        __name += __oceanic ? " Oceanic" : " Center";
}

bool
Fir::isStaffed() const
{
    return __staff->rowCount() > 0;
}

bool
Fir::isEmpty() const
{
    return __staff->rowCount() == 0;
}

void
Fir::setName(const QString& _n)
{
    __name = _n;
}

void
Fir::setCountry(const QString& _c)
{
    __country = _c;
}
