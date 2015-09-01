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
#include "network/metarupdater.h"
#include "ui/map/airportitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/firitem.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/quick/androidbridge.h"
#include "ui/quick/paletteprovider.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/metar.h"
#include "vatsimdata/pilot.h"
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
    /* Couple of QML-global instances */
    QQmlContext* ctx = __engine.rootContext();
    ctx->setContextProperty("android", new AndroidBridge(this));
    ctx->setContextProperty("palette", new PaletteProvider(this));
    ctx->setContextProperty("dp", QVariant::fromValue<qreal>(dp()));
    
    __engine.load(QUrl("qrc:///qmls/main.qml"));
    emit initialized();
    
    QObject* mapPage = findObjectByName("mapPage");
    bool c = connect(mapPage, SIGNAL(itemTouched(QVariant)), this, SLOT(__handleItemTouch(QVariant)));
    Q_ASSERT(c);
    Q_UNUSED(c);
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

void
QuickUserInterface::__handleItemTouch(const QVariant& item)
{
    Q_ASSERT(item.canConvert<MapItem*>());
    MapItem* pitem = item.value<MapItem*>();
    Q_ASSERT(pitem);
    
    Q_ASSERT(sender());
    
    if (AirportItem* ai = qobject_cast<AirportItem*>(pitem)) {
        const Airport* airport = ai->data();
        bool c = QMetaObject::invokeMethod(sender(), "showAirportDetails", Q_ARG(QVariant, QVariant::fromValue<const Airport*>(airport)));
        Q_ASSERT(c);
        Q_UNUSED(c);
    } else if (FlightItem* fi = qobject_cast<FlightItem*>(pitem)) {
        const Pilot* flight = fi->data();
        bool c = QMetaObject::invokeMethod(sender(), "showFlightDetails", Q_ARG(QVariant, QVariant::fromValue<const Pilot*>(flight)));
        Q_ASSERT(c);
        Q_UNUSED(c);
    } else if (const FirItem* fi = qobject_cast<const FirItem*>(pitem)) {
        const Fir* fir = fi->data();
        bool c = QMetaObject::invokeMethod(sender(), "showFirDetails", Q_ARG(QVariant, QVariant::fromValue<const Fir*>(fir)));
        Q_ASSERT(c);
        Q_UNUSED(c);
    } else {
        Q_UNREACHABLE();
    }
}
