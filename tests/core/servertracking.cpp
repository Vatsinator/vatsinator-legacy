/*
 * servertracking.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include "servertracking.h"
#include <core/pilot.h>
#include <core/vatsimdatareader.h>
#include <QtTest>


using namespace Vatsinator::Core;

QTEST_MAIN(ServerTracking)

void ServerTracking::initTestCase()
{
    QVERIFY(QFile(QFINDTESTDATA("data/vatsim-data_1.txt")).exists());

    m_serverTracker = new ServerTracker;
}

void ServerTracking::cleanupTestCase()
{
    delete m_serverTracker;
}

void ServerTracking::init()
{
    QString fileName = QStringLiteral("data/vatsim-data_%1.txt").arg(++m_iteration);

    VatsimDataReader reader;
    reader.read(QUrl::fromLocalFile(QFINDTESTDATA(fileName)));
    m_serverTracker->readData(reader.document());
}

void ServerTracking::cleanup()
{
    
}

void ServerTracking::iteration_0()
{
    QCOMPARE(m_serverTracker->clientCount(), 3);

    // BAW123 has no flight plan and is parked at Heathrow
    Pilot* baw123 = qobject_cast<Pilot*>(m_serverTracker->findClient("BAW123"));
    QVERIFY(baw123);

    // BAW123 is at EGLL, should be recognized
    QCOMPARE(baw123->flightPlan().departureAirport(), QString());
    QVERIFY(baw123->departure());
    QCOMPARE(baw123->departure()->icao(), QStringLiteral("EGLL"));

    // flight plan is not filled, destination airport invalid
    QCOMPARE(baw123->flightPlan().destinationAirport(), QString());
    QCOMPARE(baw123->destination(), static_cast<AirportObject*>(nullptr));

    QCOMPARE(baw123->flightPhase(), Pilot::Departing);
    QCOMPARE(baw123->progress(), 0);

    // BAW105 has complete flight plan filled and is airborne
    Pilot* baw105 = qobject_cast<Pilot*>(m_serverTracker->findClient("BAW105"));
    QVERIFY(baw105);
    QVERIFY(baw105->departure());
    QCOMPARE(baw105->departure()->icao(), QStringLiteral("EGLL"));
    QVERIFY(baw105->destination());
    QCOMPARE(baw105->destination()->icao(), QStringLiteral("OMDB"));
    QCOMPARE(baw105->flightPhase(), Pilot::Airborne);

    // AAL495 has no flight plan and is airborne (possible only on VATSIM :) )
    Pilot* aal495 = qobject_cast<Pilot*>(m_serverTracker->findClient("AAL495"));
    QVERIFY(aal495);
    QCOMPARE(aal495->departure(), static_cast<AirportObject*>(nullptr));
    QCOMPARE(aal495->destination(), static_cast<AirportObject*>(nullptr));
    QCOMPARE(aal495->flightPhase(), Pilot::Airborne);
}

void ServerTracking::iteration_1()
{
    QCOMPARE(m_serverTracker->clientCount(), 0);
}
