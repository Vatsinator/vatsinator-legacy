/*
    main.cpp
    Copyright (C) 2014  Michał Garapich michal@garapich.pl

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

#include "config.h"
#include "deviceinfo.h"
#include <gui/maprenderer.h>
#include <gui/mapscene.h>
#include <core/servertracker.h>
#include <QtQuick>
#include <QtGui>

int main(int argc, char** argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("Vatsinator"));
    QCoreApplication::setOrganizationName(QStringLiteral("VatsinatorTeam"));
    QCoreApplication::setApplicationVersion(QStringLiteral(VATSINATOR_VERSION));
    QCoreApplication::setOrganizationDomain(QStringLiteral("org.eu.vatsinator"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    qsrand(QTime::currentTime().msec());
    
    QGuiApplication app(argc, argv);
    
    QTranslator tr;
    if (tr.load(QLocale(), QStringLiteral("vatsinator"), "_", QStringLiteral("assets:/translations"))) {
        app.installTranslator(&tr);
    }
    
    QQmlApplicationEngine engine;

    DeviceInfo deviceInfo;
    engine.rootContext()->setContextProperty("deviceInfo", &deviceInfo);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
