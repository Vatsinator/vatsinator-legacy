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

#include "vatsimqmlplugin.h"
#include "modelfactory.h"
#include "core/resourcefile.h"
#include "core/pilot.h"
#include "core/servertracker.h"
#include "misc/clientlistmodel.h"
#include "misc/notamlistmodel.h"
#include <QtQml>
#include <QtGui>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace Vatsinator { namespace Imports {

static QObject* vatsim_singletontype_provider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    
    ServerTracker* serverTracker = new ServerTracker;
    serverTracker->trackServer(QUrl("https://status.vatsim.net/status.txt"));
    
    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, [serverTracker](Qt::ApplicationState state) {
        serverTracker->setUpdatesEnabled(state == Qt::ApplicationActive);    
    });
    
    return serverTracker;
}

static QObject* modelfactory_singletontype_provider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    
    return new ModelFactory;
}

void VatsimQmlPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QStringLiteral("org.eu.vatsinator.Vatsim"));
    
//     qmlRegisterUncreatableType<Client>(uri, 1, 0, "Client", "Client instances can be created only internally");
    qmlRegisterUncreatableType<Pilot>(uri, 1, 0, "Pilot", "Pilot instances can be created only internally");
    
    qmlRegisterSingletonType<ServerTracker>(uri, 1, 0, "ServerTracker", &vatsim_singletontype_provider);
    qmlRegisterSingletonType<ModelFactory>(uri, 1, 0, "ModelFactory", &modelfactory_singletontype_provider);
    
    qmlRegisterType<ClientListModel>(uri, 1, 0, "ClientListModel");
    qmlRegisterType<NotamListModel>(uri, 1, 0, "NotamListModel");
}

}} /* namespace Vatsinator::Imports */
