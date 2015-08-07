/*
 * quickuserinterface.cpp
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
#include <QtQuick>

#include "events/decisionevent.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/quick/androidbridge.h"
#include "ui/quick/map.h"
#include "ui/quick/paletteprovider.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "quickuserinterface.h"

QuickUserInterface::QuickUserInterface(QObject* parent) :
    UserInterface(parent)
{
    
}

QObject*
QuickUserInterface::rootItem()
{
    auto objects = __engine.rootObjects();
    for (QObject* o: objects) {
        if (o->objectName() == "vatsinatorWindow")
            return o;
    }
    
    return nullptr;
}

QObject*
QuickUserInterface::findObjectByName(const QString& name)
{
    QObject* root = qui()->rootItem();
    if (root)
        return root->findChild<QObject*>(name);
    else
        return nullptr;
}

qreal
QuickUserInterface::dp() const
{
    return QGuiApplication::primaryScreen()->physicalDotsPerInch() / 160;
}

void
QuickUserInterface::initialize()
{
    qmlRegisterType<Airport>();
    
    qmlRegisterType<Map>("org.eu.vatsinator.ui", 1, 0, "Map");
    
    QQmlContext* ctx = __engine.rootContext();
    ctx->setContextProperty("flights", vApp()->vatsimDataHandler()->flights());
    ctx->setContextProperty("atcs", vApp()->vatsimDataHandler()->atcs());
    ctx->setContextProperty("android", new AndroidBridge(this));
    ctx->setContextProperty("palette", new PaletteProvider(this));
    ctx->setContextProperty("dp", QVariant::fromValue<qreal>(dp()));
    
    __engine.load(QUrl("qrc:///qmls/main.qml"));
    emit initialized();
}

void
QuickUserInterface::fatal(const QString& message)
{
    Q_UNUSED(message);
}

void
QuickUserInterface::warning(const QString& message)
{
    Q_UNUSED(message);
}

void
QuickUserInterface::showDetails(const Airport* airport)
{
    Q_UNUSED(airport);
}

void
QuickUserInterface::showDetails(const Client* client)
{
    Q_UNUSED(client);
}

void
QuickUserInterface::showDetails(const Fir* fir)
{
    Q_UNUSED(fir);
}

void
QuickUserInterface::showMetar(const QString& metar)
{
    Q_UNUSED(metar);
}

void
QuickUserInterface::showStatsDialog()
{
    // enable stats anyway temporarily
    DecisionEvent* e = new DecisionEvent("statistics", DecisionEvent::Accepted);
    QCoreApplication::postEvent(vApp(), e);
}

void
QuickUserInterface::ensureMainWindowIsActive()
{

}

void
QuickUserInterface::vatsimEvent(VatsimEvent* event)
{
    Q_UNUSED(event);
}
