/*
 * tmaarea.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "tmaarea.h"
#include "core/atc.h"

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

TmaArea::TmaArea(Core::AirportObject* airport, QList<LonLat> boundary, MapScene* scene, QObject* parent) :
    MapArea(scene, parent),
    m_airport(airport),
    m_boundingRect(QPointF(0.0, 0.0), QSizeF(2.0, -2.0)),
    m_boundary(boundary)
{
    m_boundingRect.moveCenter(airport->position());
    
    connect(m_airport, &ClientList::clientCountChanged, this, &TmaArea::refreshVisible);
    refreshVisible();
}

QRectF TmaArea::boundingRect() const
{
    return m_boundingRect;
}

bool TmaArea::isVisible() const
{
    return m_visible;
}

void TmaArea::draw(WorldPainter* painter, MapDrawable::DrawFlags flags) const
{
    Q_UNUSED(flags);
    
    painter->save();
    
    painter->setBrush(QColor(62, 87, 127, 50));
    painter->setPen(QColor(62, 87, 127, 50));
    
//    if (m_boundary.isEmpty()) {
//        QRect rect = m_boundingRect * transform;
//        QPoint c = rect.center();
//        int width = qMax(rect.width(), 44);
//        rect.setSize(QSize(width, width));
//        rect.moveCenter(c);
//        painter->drawEllipse(rect);
//    } else {
//        auto region = transform.map(m_boundary.begin(), m_boundary.end());
//        painter->drawPolygon(region.data(), region.length());
//    }

    if (m_boundary.isEmpty()) {
        // TODO
    } else {
        painter->drawPolygon(m_boundary);
    }
    
    painter->restore();
}

void TmaArea::refreshVisible()
{
    auto atcs = m_airport->clients<Atc>();
    m_visible = std::any_of(atcs.begin(), atcs.end(), [](auto it) {
        return it->facility() == Atc::App;
    });
}

}} /* namespace Vatsinator::Gui */
