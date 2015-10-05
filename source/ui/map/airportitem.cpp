/*
 * airportitem.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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
#include "models/atctablemodel.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/map/tmaarea.h"
#include "ui/map/pixmapprovider.h"
#include "ui/userinterface.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/tma.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "airportitem.h"

// pixmaps to use, %1 is DPI prefix
static const QString AirportInactivePixmap = QStringLiteral(":/pixmaps/%1/airport_inactive.png");
static const QString AirportActivePixmap = QStringLiteral(":/pixmaps/%1/airport.png");
static const QString AirportActiveWithAtcPixmap = QStringLiteral(":/pixmaps/%1/airport_staffed.png");


AirportItem::AirportItem(const Airport* airport, QObject* parent) :
    MapItem(parent),
    __scene(qobject_cast<MapScene *>(parent)),
    __airport(airport),
    __tma(nullptr)
{
    connect(airport, &Airport::updated, this, &AirportItem::__invalidate);
    
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged, [this]() {
        __label = QPixmap();
    });
}

bool
AirportItem::isVisible() const
{
    return data()->isEmpty() ? __scene->settings().view.empty_airports : __scene->settings().view.airports_layer;
}

LonLat
AirportItem::position() const
{
    return __airport->position();
}

void
AirportItem::draw(QPainter* painter, const WorldTransform& transform, DrawFlags flags) const
{
    if (__icon.isNull()) {
        __loadIcon();
    }
    
    if (flags & DrawSelected)
        __drawLines(painter, transform);
    
    QPoint center = position() * transform;
    
    QRect rect(QPoint(0, 0), __icon.size());
    rect.moveCenter(center);
    rect.moveBottom(rect.bottom() - rect.height() / 2);
    
    painter->drawPixmap(rect, __icon);
    
    if (__scene->settings().view.airport_labels) {
        if (__label.isNull())
            __prepareLabel();
        
        QRect rect2(__label.rect());
        rect2.moveCenter(center);
        rect2.moveBottom(rect2.bottom() + rect2.height() / 2);
        
        painter->drawPixmap(rect2, __label);
    }
}

int
AirportItem::z() const
{
    return 2;
}

void
AirportItem::__loadIcon() const
{
    if (data()->isEmpty()) {
        if (!QPixmapCache::find(AirportInactivePixmap.arg(MapConfig::generalizedDensity()), &__icon)) {
            __icon.load(AirportInactivePixmap.arg(MapConfig::generalizedDensity()));
            QPixmapCache::insert(AirportInactivePixmap.arg(MapConfig::generalizedDensity()), __icon);
        }
    } else {
        if (data()->staff()->toList().isEmpty()) {
            if (!QPixmapCache::find(AirportActivePixmap.arg(MapConfig::generalizedDensity()), &__icon)) {
                __icon.load(AirportActivePixmap.arg(MapConfig::generalizedDensity()));
                QPixmapCache::insert(AirportActivePixmap.arg(MapConfig::generalizedDensity()), __icon);
            }
        } else {
            if (!QPixmapCache::find(AirportActiveWithAtcPixmap.arg(MapConfig::generalizedDensity()), &__icon)) {
                __icon.load(AirportActiveWithAtcPixmap.arg(MapConfig::generalizedDensity()));
                QPixmapCache::insert(AirportActiveWithAtcPixmap.arg(MapConfig::generalizedDensity()), __icon);
            }
        }
    }
}

void
AirportItem::__prepareLabel() const
{
    __label = __scene->pixmapProvider()->backgroundForAirportLabel();
    
    QPainter p(&__label);
    
    QFont font = SettingsManager::get("map.airport_font").value<QFont>();
    p.setFont(font);
    
    QPen pen(QColor(245, 245, 245));
    p.setPen(pen);
    
    p.drawText(__label.rect(), Qt::AlignCenter, data()->icao());
    p.end();
}

void
AirportItem::__drawLines(QPainter* painter, const WorldTransform& transform) const
{
    QPainter::RenderHints hints = painter->renderHints();
    painter->setRenderHints(hints | QPainter::Antialiasing);
    
    QPen orig = painter->pen();
    QPoint pos = position() * transform;
    
    painter->setPen(QPen(QColor(3, 116, 164)));
    for (const Pilot* p: data()->outbounds()->toList()) {
        if (p->phase() == Pilot::Airborne || p->phase() == Pilot::Arrived) {
            QPoint pf = p->position() * transform;
            painter->drawLine(pf, pos);
        }
    }
    
    QPen pen(QColor(133, 164, 164));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    
    for (const Pilot* p: data()->inbounds()->toList()) {
        if (p->phase() == Pilot::Airborne || p->phase() == Pilot::Departing) {
            QPoint pf = p->position() * transform;
            painter->drawLine(pf, pos);
        }
    }
    
    painter->setRenderHints(hints);
    painter->setPen(orig);
}

void
AirportItem::__invalidate()
{
    if (!QCoreApplication::closingDown())
        __icon = QPixmap();
    
    if (data()->facilities() & Controller::App) {
        if (!__tma) {
            __tma = new TmaArea(data(), __scene);
            __scene->addArea(__tma);
        }
    } else {
        if (__tma) {
            __scene->removeArea(__tma);
            __tma->deleteLater();
            __tma = nullptr;
        }
    }
}
