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
#include "ui/quick/screenimageprovider.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "quickuserinterface.h"

QuickUserInterface::QuickUserInterface(QObject* parent) :
    UserInterface(parent)
{
    
}

QuickUserInterface::~QuickUserInterface()
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
    qreal dp = QGuiApplication::primaryScreen()->physicalDotsPerInch() / 160;
    ctx->setContextProperty("dp", QVariant::fromValue<qreal>(dp));
    
    __engine.addImageProvider("screen", new ScreenImageProvider);
    
    __engine.load(QUrl("qrc:///qmls/main.qml"));
    
    /* TODO Cache map from time to time */
    
    emit initialized();
}

void
QuickUserInterface::fatal(const QString&)
{

}

void
QuickUserInterface::warning(const QString&)
{

}

void
QuickUserInterface::statusError()
{

}

void
QuickUserInterface::dataError()
{

}

void
QuickUserInterface::showVatsimMessage(const QString&)
{

}

void
QuickUserInterface::showDetails(const Airport*)
{

}

void
QuickUserInterface::showDetails(const Client*)
{

}

void
QuickUserInterface::showDetails(const Fir*)
{

}

void
QuickUserInterface::showMetar(const QString&)
{

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