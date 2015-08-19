/*
 * flightitem.cpp
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
#include "ui/map/mapscene.h"
#include "ui/map/modelpixmapprovider.h"
#include "ui/userinterface.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "flightitem.h"

FlightItem::FlightItem(const Pilot* pilot, QObject* parent) :
    MapItem(parent),
    __scene(qobject_cast<MapScene*>(parent)),
    __pilot(pilot)
{
    connect(pilot, &Pilot::aircraftChanged, this, &FlightItem::__invalidateModel);
}

bool
FlightItem::isVisible() const
{
    return data()->phase() == Pilot::Airborne && !data()->isPrefiledOnly();
}

LonLat
FlightItem::position() const
{
    return __pilot->position();
}

void
FlightItem::draw(QPainter* painter, const WorldTransform& transform, DrawFlags flags) const
{
    if (__model.isNull())
        __prepareModel();
    Q_ASSERT(!__model.isNull());
    
    QPoint pos = position() * transform;
    
    if (flags & DrawSelected)
        __drawLines(painter, transform);
    
    QRect rect(QPoint(0, 0), __model.size());
    rect.moveCenter(pos);
    painter->drawPixmap(rect, __model);
}

void
FlightItem::__prepareModel() const
{
    QTransform t;
    t.rotate(static_cast<qreal>(data()->heading()));
    __model = __scene->modelPixmapProvider()->pixmapForModel(__pilot->aircraft())
        .transformed(t, Qt::SmoothTransformation);
    
    /* We drop shadow after transformation, it looks more convincing */
    __dropShadow(&__model);
}

void
FlightItem::__drawLines(QPainter* painter, const WorldTransform& transform) const
{
    QPainter::RenderHints hints = painter->renderHints();
    painter->setRenderHints(hints | QPainter::Antialiasing);
    
    QPen orig = painter->pen();
    QPoint pos = position() * transform;
    
    if (data()->origin()->isValid()) {
        painter->setPen(QPen(__scene->settings().colors.origin_to_pilot_line));
        QPoint p = data()->origin()->position() * transform;
        painter->drawLine(p, pos);
    }
    
    if (data()->destination()->isValid()) {
        QPen pen(__scene->settings().colors.pilot_to_destination);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        QPoint p = data()->destination()->position() * transform;
        painter->drawLine(p, pos);
    }
    
    painter->setRenderHints(hints);
    painter->setPen(orig);
}

void
FlightItem::__dropShadow(QPixmap* image) const
{
    QPixmap orig = image->copy();
    QBitmap mask = image->mask();
    
    image->fill(Qt::transparent);
    QRect target = image->rect();
    target.moveBottom(target.bottom() + 4);
    
    QPainter p(image);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.setPen(QColor(180, 180, 180, 100));
    p.drawPixmap(target, mask, mask.rect());
    p.setPen(QPen());
    p.drawPixmap(image->rect(), orig, orig.rect());
    p.end();
}

void
FlightItem::__invalidateModel()
{
    __model = QPixmap();
}
