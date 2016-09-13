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
#include <QtTest>

constexpr quint32 Pid1 = 17161514;
constexpr quint32 Pid2 = 16151412;

using namespace Vatsinator::Core;

QTEST_MAIN(ServerTracking)

void ServerTracking::initTestCase()
{
    m_serverTracker = new ServerTracker;
}

void ServerTracking::cleanupTestCase()
{
    delete m_serverTracker;
}

void ServerTracking::init()
{
    // Called before each testfunction is executed
}

void ServerTracking::cleanup()
{
    
}

void ServerTracking::findClient()
{
    Pilot* p1 = new Pilot(Pid1, m_serverTracker);
    p1->setCallsign("p1");
    p1->setPosition(LonLat(51.4775, -0.461389));
    m_serverTracker->addClient(p1);
    QVERIFY(m_serverTracker->findClient("p1") == p1);
}
