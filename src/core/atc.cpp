/*
 * atc.cpp
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

#include "atc.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

Atc::Atc(quint32 pid, ServerTracker* server) :
    Client(pid, server)
{
    connect(this, &Atc::airportChanged, &Atc::updateDescription);
    connect(this, &Atc::firChanged, this, &Atc::updateDescription);
}

Atc::~Atc() {}

void Atc::setRating(Atc::Rating rating)
{
    if (m_rating != rating) {
        m_rating = rating;
        emit ratingChanged(m_rating);
    }
}

void Atc::setFrequency(const QString& frequency)
{
    if (m_frequency != frequency) {
        m_frequency = frequency;
        emit frequencyChanged(m_frequency);
    }
}

void Atc::setAtis(const QString& atis)
{
    if (m_atis != atis) {
        m_atis = cleanupAtis(atis);
        emit atisChanged(m_atis);
    }
}

void Atc::setFacility(Atc::Facility facility)
{
    if (m_facility != facility) {
        m_facility = facility;
        emit facilityChanged(m_facility);
    }
}

void Atc::setDescription(const QString& description)
{
    if (m_description != description) {
        m_description = description;
        emit descriptionChanged(m_description);
    }
}

void Atc::setAirport(AirportObject* airport)
{
    if (m_airport != airport) {
        m_airport = airport;
        emit airportChanged(airport);
    }
}

void Atc::setFir(FirObject* fir)
{
    if (m_fir != fir) {
        m_fir = fir;
        emit firChanged(fir);
    }
}

void Atc::setIsUir(bool isUir)
{
    m_uir = isUir;
}

QString Atc::facilityStr(Atc::Facility facility)
{
    switch (facility) {
        case Atis:
            return QStringLiteral("ATIS");
        case Del:
            return QStringLiteral("Delivery");
        case Gnd:
            return QStringLiteral("Ground");
        case Twr:
            return QStringLiteral("Tower");
        case App:
            return QStringLiteral("Approach");
        case Dep:
            return QStringLiteral("Departure");
        default:
            return QString();
    }
}

QString Atc::ratingStr(Atc::Rating rating)
{
    switch (rating) {
        case Observer:
            return QStringLiteral("Observer");
        case TowerTrainee:
            return QStringLiteral("Tower Trainee (S1)");
        case TowerController:
            return QStringLiteral("Tower Controller (S2)");
        case TmaController:
            return QStringLiteral("TMA Controller (S3)");
        case EnrouteController:
            return QStringLiteral("Enroute Controller (C1)");
        case SeniorController:
            return QStringLiteral("Senior Controller (C3)");
        default:
            return QString();
    }
}

QString Atc::cleanupAtis(const QString& orig)
{
    return orig.mid(2).replace(QRegularExpression("\\^."), "\n");
}

void Atc::updateDescription()
{
    if (fir()) {
        setDescription(fir()->name());
    } else if (airport()) {
        if (airport()->isKnownAirport()) {
            QString ap;
            if (airport()->name() != airport()->city()) {
                ap = airport()->city() % "/" % airport()->name();
            } else {
                ap = airport()->name();
            }
            
            setDescription(ap % " " % facilityStr(facility()));
        } else {
            setDescription(QStringLiteral("Unknown ") % facilityStr(facility()));
        }
    }
}

}} /* namespace Vatsinator::Core */
