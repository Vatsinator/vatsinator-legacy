/*
 * atctracker.h
 * Copyright (C) 2017  Michał Garapich <michal@garapich.pl>
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

#include "atctracker.h"
#include "airportmodel.h"
#include "geo.h"
#include "servertracker.h"

namespace Vatsinator { namespace Core {

/* Maximum distance from ATC position to the airport it controls */
constexpr qreal MaxAtcToAirport = 0.5;


AtcTracker::AtcTracker(Atc* atc, QObject* parent) :
    QObject(parent),
    m_atc(atc)
{
    connect(m_atc, &QObject::destroyed, this, &QObject::deleteLater);
    initialize();
}

void AtcTracker::initialize()
{
    switch (atc()->facility()) {
        case Atc::Atis:
        case Atc::Del:
        case Atc::Gnd:
        case Atc::Twr:
        case Atc::Dep:
        case Atc::App: {
            Airport* ap = nullptr;
            if (atc()->icao.isEmpty()) {
                ap = atc()->server()->airports()->nearest(atc()->position());
                if (nmDistance(atc()->position(), ap->position()) < MaxAtcToAirport) {
                    atc()->icao = ap->icao();
                } else {
                    qFatal("No airport for ATC (%s)", qPrintable(atc()->callsign()));
                }
            } else {
                ap = atc()->server()->airports()->findByIcao(atc()->icao);
            }

            atc()->setAirport(ap);
            atc()->setFir(nullptr);
            break;
        }

        case Atc::Ctr:
        case Atc::Fss: {
            Q_ASSERT(!atc()->icao.isEmpty());
            if (Fir* f = atc()->server()->firs()->findByIcao(atc()->icao)) {
                atc()->setFir(f);
//                atc()->setFir(f);
//            } else {
//                bool found;
//                UirListReader::UirData uir;
//                std::tie(found, uir) = atc()->server()->context()->uirs()->find(atc()->icao);
//                if (found) {
//                    atc()->setDescription(std::get<1>(uir));
//                    atc()->setIsUir(true);

//                    for (const QString& icao: std::get<2>(uir)) {
//                        FirObject* f = atc()->server()->firObject(icao);
//                        if (f)
//                            f->add(atc());
//                    }
//                } else {
//                    qWarning("No FIR for ATC (%s)", qPrintable(atc()->callsign()));
//                }
            } else {
                qWarning("No FIR for ATC (%s)", qPrintable(atc()->callsign()));
            }

            atc()->setAirport(nullptr);
            break;
        }

        case Atc::Obs:
            atc()->setAirport(nullptr);
            atc()->setFir(nullptr);
            break;
    }
}

}} /* namespace Vatsinator::Core */
