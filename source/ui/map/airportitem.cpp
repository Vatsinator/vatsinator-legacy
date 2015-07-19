/*
 * airportitem.cpp
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#include <QtGui>

#include "db/airportdatabase.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/userinterface.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/tma.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "airportitem.h"

// pixmaps to use
static const QString AirportInactivePixmap = QStringLiteral(":/pixmaps/airport_inactive.png");
static const QString AirportActivePixmap = QStringLiteral(":/pixmaps/airport.png");
static const QString AirportActiveWithAtcPixmap = QStringLiteral(":/pixmaps/airport_staffed.png");

AirportItem::AirportItem(const Airport* airport, QObject* parent) :
    MapItem(parent),
    __scene(qobject_cast<MapScene * >(parent)),
    __airport(airport),
    __position(airport->data()->longitude, airport->data()->latitude),
    __label(airport->icao())
{
    
}

AirportItem::~AirportItem()
{
    
}

bool
AirportItem::isVisible() const
{
    Q_ASSERT(!__position.isNull());
    
    if (data()->isEmpty())
        return __scene->settings().view.empty_airports;
    else
        return __scene->settings().view.airports_layer;
}

bool
AirportItem::isLabelVisible() const
{
    return __scene->settings().view.airport_labels;
}

const LonLat&
AirportItem::position() const
{
    return __position;
}

void
AirportItem::draw(QPainter* painter, const WorldTransform& transform) const
{
    if (__icon.isNull()) {
        if (data()->isEmpty()) {
            __icon.load(AirportInactivePixmap);
        } else {
            if (data()->staff()->staff().isEmpty())
                __icon.load(AirportActivePixmap);
            else
                __icon.load(AirportActiveWithAtcPixmap);
        }
    }
    
    QRect rect(QPoint(0, 0), __icon.size());
    rect.moveCenter(position() * transform);
    
    
    painter->drawPixmap(rect, __icon);
    
//     QRectF textRect(QPointF(0.0, 0.0), __label.size());
//     textRect.moveCenter(rect.center());
//     textRect.moveTop(rect.bottom());
//     painter->drawStaticText(textRect.topLeft(), __label);
}

QString
AirportItem::tooltipText() const
{
    QString desc = QString("%1 %2, %3").arg(data()->icao(), data()->name(), data()->city());
                       
    QString staff, deparr;
    
    if (!data()->isEmpty()) {
        for (const Controller* c : data()->staff()->staff()) {
            staff.append("<br>");
            staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
        }
        
        int deps = data()->countDepartures();
        
        if (deps > 0) {
            deparr.append("<br>");
            deparr.append(tr("Departures: %1").arg(QString::number(deps)));
        }
        
        int arrs = data()->countArrivals();
        
        if (arrs > 0) {
            deparr.append("<br>");
            deparr.append(tr("Arrivals: %1").arg(QString::number(arrs)));
        }
    }
    
    return QString("<p style='white-space:nowrap'><center>" % desc % staff % deparr % "</center></p>");
}

void
AirportItem::showDetails() const
{
    vApp()->userInterface()->showDetails(data());
}
