/*
 * widgetsprivate.cpp
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

#include "widgetsprivate.h"
#include "core/atc.h"
#include "core/airportobject.h"
#include "gui/airportitem.h"
#include "gui/firitem.h"
#include "gui/flightitem.h"
#include <QtWidgets>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;

namespace Vatsinator { namespace Widgets {

QString makeToolTip(const MapItem* item)
{
    QString text;
    
    if (const AirportItem* airportItem = qobject_cast<const AirportItem*>(item)) {
        const AirportObject* ap = airportItem->airport();
        QString desc = QStringLiteral("%1 %2, %3").arg(ap->icao(), ap->name(), ap->city());
        
        int n = ap->inboundFlightsCount();
        if (n)
            desc = desc % QStringLiteral("<br />") % qApp->translate("MapWidget", "Inbound flights: %1").arg(QString::number(n));
        n = ap->outboundFlightsCount();
        if (n)
            desc = desc % QStringLiteral("<br />") % qApp->translate("MapWidget", "Outbound flights: %1").arg(QString::number(n));
        
        text = desc;
    } else if (const FlightItem* flightItem = qobject_cast<const FlightItem*>(item)) {
        auto pilot = flightItem->pilot();
        
        QString desc = QStringLiteral("%1 (%2)").arg(pilot->realName(), pilot->aircraft());
        QString fromto = pilot->departure()->representativeName() % QStringLiteral(" &raquo; ") % pilot->destination()->representativeName();
        QString gs = qApp->translate("MapWidget", "Ground speed: %1 knots").arg(QString::number(pilot->groundSpeed()));
        QString alt = qApp->translate("MapWidget", "Altitude: %1 feet").arg(QString::number(pilot->altitude()));
        
        text = QStringList({pilot->callsign(), desc, fromto, gs, alt}).join("<br />");
    } else if (const FirItem* firItem = qobject_cast<const FirItem*>(item)) {
        auto fir = firItem->fir();
        
        QString desc;
        if (!fir->name().isEmpty()) {
            desc.append(fir->name());
            if (!fir->country().isEmpty()) {
                desc.append(", ").append(fir->country());
            }
        } else {
            desc.append(fir->icao());
        }
        
        auto atcs = fir->clients<Atc>();
        QString staff;
        for (auto a: atcs) {
            staff.append(QStringLiteral("<br>%1 %2 %3").arg(a->callsign(), a->frequency(), a->realName()));
        }
        
        text = desc + staff;
    }
    
    return QStringLiteral("<p style='white-space:nowrap'><center>") % text % QStringLiteral("</center></p>");
}

void fixupGeometry(QWidget* window)
{
    if (!window->isWindow())
        return;
    
    window->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, window->size(),
                                            QDesktopWidget().screenGeometry(QApplication::activeWindow())));
}

}} /* namespace Vatsinator::Widgets */
