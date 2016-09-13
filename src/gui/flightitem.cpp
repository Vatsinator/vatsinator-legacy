/*
 * flightitem.cpp
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

#include "flightitem.h"
#include "mapscene.h"
#include "pixmapprovider.h"
#include "worldtransform.h"
#include "core/geo.h"
#include <QtGui>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

FlightItem::FlightItem(Pilot* pilot, MapScene* scene,
                       QObject* parent) :
    MapItem(scene, parent),
    m_pilot(pilot)
{
    connect(m_pilot, &Client::positionChanged, this, &MapItem::updated);
    connect(m_pilot, &Pilot::headingChanged, this, &FlightItem::invalidateModel);
    connect(m_pilot, &Pilot::headingChanged, this, &MapItem::updated);
    connect(m_pilot, &Pilot::flightPhaseChanged, this, &MapItem::updated);
    connect(m_pilot, &Client::positionChanged, this, &FlightItem::clearNodes);
    connect(m_pilot, &Pilot::routeChanged, this, &FlightItem::clearNodes);
}

bool FlightItem::isVisible() const
{
    return m_pilot->flightPhase() == Pilot::Airborne;
}

LonLat FlightItem::position() const
{
    return m_pilot->position();
}

QSize FlightItem::size() const
{
    if (m_model.isNull())
        prepareModel();

    Q_ASSERT(!m_model.isNull());
    return m_model.size();
}

void FlightItem::draw(WorldPainter* painter, DrawFlags flags) const
{
//    if (flags & Debug) {
//        painter->drawRect(rectForModel(transform));
        
//        if (flags & DrawSelected) {
//            painter->fillRect(rectForModel(transform), QBrush(QColor(100, 100, 100, 100)));
//        }
//    }

    if (flags & DrawSelected) {
        painter->save();
        fetchNodes();

        painter->setPen(QPen(QColor(3, 116, 164)));
        painter->drawPolyline(m_depToPilotNodes);

        QPen pen(QColor(133, 164, 164));
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawPolyline(m_pilotToDestNodes);

        painter->restore();
    }

    if (m_model.isNull())
        prepareModel();

    Q_ASSERT(!m_model.isNull());
    painter->drawPixmap(position(), m_model);
}

int FlightItem::z() const
{
    return 3;
}

void FlightItem::prepareModel() const
{
    Q_ASSERT(scene());
    QTransform t;
    t.rotate(static_cast<qreal>(m_pilot->heading()));
    QPixmap orig = scene()->pixmapProvider()->pixmapForModel(m_pilot->aircraft());
    m_model = orig.transformed(t, Qt::SmoothTransformation);

    dropShadow(&m_model);
    
    if (m_model.devicePixelRatio() != orig.devicePixelRatio()) // qt bug?
        m_model.setDevicePixelRatio(orig.devicePixelRatio());
}

void FlightItem::prepareLabel() const
{
    m_label = scene()->pixmapProvider()->flightTooltipBackground();

    QPainter p(&m_label);
    QPen pen(QColor(0, 0, 0));
    p.setPen(pen);

    QRect rect = m_label.rect();
    rect.setBottom(rect.bottom() - 8 * qApp->primaryScreen()->devicePixelRatio() - 2);
    p.drawText(rect, Qt::AlignCenter, pilot()->callsign());
    p.end();
}

void FlightItem::fetchNodes() const
{
    if (m_depToPilotNodes.isEmpty())
        m_depToPilotNodes = pilot()->nodes(Pilot::DepartureToPilot);
    
    if (m_pilotToDestNodes.isEmpty())
        m_pilotToDestNodes = pilot()->nodes(Pilot::PilotToDestination);
    
    auto fixCrossIdl = [](QList<LonLat>& points, const LonLat& p1) {
        qreal p1Sign = p1.longitude() / qFabs(p1.longitude());
        
        for (int i = 0; i < points.size(); ++i) {
            qreal offset = .0;
            LonLat& p2 = points[i];
            
            /**
             * Checks whether the given line crosses the IDL (International Date Line).
             * For reference:
             * https://github.com/Vatsinator/Vatsinator/pull/2
             * Code by @Ucchi98.
             */
            qreal p2Sign = p2.longitude() / qFabs(p2.longitude());
            
            if (p1Sign != p2Sign) {
                qreal dst = nmDistance(p1, p2);
                
                if (p1Sign < 0 && dst > nmDistance(p1 + LonLat(360.0, .0), p2))
                    offset -= 360.0;
                else if (p1Sign >= 0 && dst > nmDistance(p1, p2 + LonLat(360.0, .0)))
                    offset += 360.0;
            }
            
            p2.rx() += offset;
        }
    };
    
    fixCrossIdl(m_depToPilotNodes, pilot()->position());
    fixCrossIdl(m_pilotToDestNodes, pilot()->position());
}

void FlightItem::dropShadow(QPixmap* image)
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

void FlightItem::invalidateModel()
{
    m_model = QPixmap();
}

void FlightItem::clearNodes()
{
    m_depToPilotNodes.clear();
    m_pilotToDestNodes.clear();
}

}} /* namespace Vatsinator::Gui */
