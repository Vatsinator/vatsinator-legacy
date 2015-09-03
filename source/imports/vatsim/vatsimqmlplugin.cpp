/*
 * vatsimqmlplugin.cpp
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

#include <QtQml>

#include "models/atctablemodel.h"
#include "models/flighttablemodel.h"
#include "network/metarupdater.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/metar.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "vatsimqmlplugin.h"

void
VatsimQmlPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_ASSERT(uri == QStringLiteral("Vatsim"));
    
    QQmlContext* ctx = engine->rootContext();
    ctx->setContextProperty("flights", vApp()->vatsimDataHandler()->flights());
    ctx->setContextProperty("atcs", vApp()->vatsimDataHandler()->atcs());
    ctx->setContextProperty("metars", vApp()->metarUpdater());
}

void
VatsimQmlPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QStringLiteral("Vatsim"));
    
    qmlRegisterUncreatableType<const Airport>(uri, 1, 0, "Airport", "Airport instances can be created only by Vatsinator, internally");
    qmlRegisterUncreatableType<const Fir>(uri, 1, 0, "Fir", "Fir instances can be created only during initialization");
    qmlRegisterUncreatableType<const Pilot>(uri, 1, 0, "Pilot", "Pilot instances can be created only by Vatsinator, internally");
    qmlRegisterType<Metar>(uri, 1, 0, "Metar");
}
