/*
 * airportitem.cpp
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

#include "airportitem.h"
#include "mapscene.h"
#include "pixmapprovider.h"
#include "worldtransform.h"
#include "core/atc.h"
#include <QtGui>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

AirportItem::AirportItem(Airport *airport, MapScene* scene, QObject* parent) :
    MapItem(scene, parent),
    m_airport(airport)
{
    connect(m_airport, &ClientList::clientCountChanged, this, &AirportItem::removeIcon);
}

bool AirportItem::isVisible() const
{
    return m_airport->count() > 0;
}

Core::LonLat AirportItem::position() const
{
    return m_airport->position();
}

QSize AirportItem::size() const
{
    if (m_icon.isNull())
        prepareIcon();

    Q_ASSERT(!m_icon.isNull());
    return m_icon.size() / qApp->primaryScreen()->devicePixelRatio();
}

void AirportItem::draw(WorldPainter* painter, MapItem::DrawFlags flags) const
{
//    if (flags & Debug) {
//        painter->drawRect(rectForIcon(transform));
        
//        if (flags & DrawSelected) {
//            painter->fillRect(rectForIcon(transform), QBrush(QColor(100, 100, 100, 100)));
//        }
//    }

    if (flags & DrawSelected) {
        painter->save();
        painter->setPen(QPen(QColor(3, 116, 164)));

        QList<const Pilot*> flights = airport()->outboundFlights();
        for (const Pilot* p: flights) {
            if (p->flightPhase() != Pilot::Departing)
                painter->drawLine(airport()->position(), p->position());
        }

        QPen pen(QColor(133, 164, 164));
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);

        flights = airport()->inboundFlights();
        for (const Pilot* p: flights) {
            if (p->flightPhase() != Pilot::Arrived)
                painter->drawLine(airport()->position(), p->position());
        }

        painter->restore();
    }

    if (m_icon.isNull())
        prepareIcon();

    Q_ASSERT(!m_icon.isNull());
    painter->drawPixmap(airport()->position(), m_icon);
}

int AirportItem::z() const
{
    return 2;
}

void AirportItem::prepareIcon() const
{
    bool staffed = m_airport->hasClientOfType<Atc>();
    m_icon = scene()->pixmapProvider()->pixmapForAirport(staffed);
}

void AirportItem::prepareLabel() const
{
    QFont font;
    font.setPointSize(font.pointSize() - 2);
//    font.setBold(true);

    m_label = scene()->pixmapProvider()->airportLabelBackground(font);

    QPainter p(&m_label);
    p.setPen(QPen(QColor(245, 245, 245)));
    p.setFont(font);

    p.drawText(m_label.rect(), Qt::AlignCenter, airport()->icao());
    p.end();
}

void AirportItem::removeIcon()
{
    m_icon = QPixmap();
}


}} /* namespace Vatsinator::Gui */
