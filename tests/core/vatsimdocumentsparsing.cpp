/*
 * vatsimdocumentsparsing.cpp
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

#include "vatsimdocumentsparsing.h"
#include "core/vatsimdatareader.h"
#include "core/vatsimstatusreader.h"
#include <QtTest>

using namespace Vatsinator::Core;

QTEST_MAIN(VatsimDocumentsParsing)

void VatsimDocumentsParsing::initTestCase()
{
    // Called before the first testfunction is executed
}

void VatsimDocumentsParsing::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void VatsimDocumentsParsing::init()
{
    // Called before each testfunction is executed
}

void VatsimDocumentsParsing::cleanup()
{
    // Called after every testfunction
}

void VatsimDocumentsParsing::parsesData()
{
    VatsimDataReader reader;
    QString fileName = QFINDTESTDATA("vatsim-data.txt");
    QVERIFY(QFile(fileName).exists());

    QSignalSpy spy(&reader, SIGNAL(dataRead(Vatsinator::Core::VatsimDataDocument)));
    reader.read(QUrl::fromLocalFile(fileName));
    QCOMPARE(spy.count(), 1);

    QList<QVariant> args = spy.takeFirst();
    QVERIFY(args.at(0).canConvert<VatsimDataDocument>());

    VatsimDataDocument document = args.at(0).value<VatsimDataDocument>();
    QCOMPARE(document.connectedClients(), 441);
    QCOMPARE(document.clients().length(), 441);
    QCOMPARE(document.reload(), 2);
}

void VatsimDocumentsParsing::parsesStatus()
{
    VatsimStatusReader reader;
    QString fileName = QFINDTESTDATA("status-3.txt");
    QVERIFY(QFile(fileName).exists());

    QSignalSpy spy(&reader, SIGNAL(statusRead(Vatsinator::Core::VatsimStatusDocument)));
    reader.read(QUrl::fromLocalFile(fileName));
    QCOMPARE(spy.count(), 1);

    QList<QVariant> args = spy.takeFirst();
    QVERIFY(args.at(0).canConvert<VatsimStatusDocument>());

    VatsimStatusDocument document = args.at(0).value<VatsimStatusDocument>();
    QVERIFY(document.isValid());
    QCOMPARE(document.dataFileUrls().length(), 4);
    QCOMPARE(document.serversListUrls().length(), 4);
    QCOMPARE(document.metar(), QUrl("http://metar.vatsim.net/metar.php"));
}
