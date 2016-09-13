/*
 * firarea.cpp
 * Copyright (C) 2015 Michał Garapich <michal@garapich.pl>
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

#include "firarea.h"
#include "core/atc.h"
#include <QtGui>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

FirArea::FirArea(Core::FirObject* fir, Core::Fir::Boundaries boundaries,
                 MapScene* scene, QObject* parent) :
    MapArea(scene, parent),
    m_fir(fir),
    m_polygon(boundaries.toVector())
{
    if (m_polygon.first() != m_polygon.last()) {
        m_polygon.append(m_polygon.first());
    }
    
    m_boundary.reserve(m_polygon.count() * 2);
    for (auto p: m_polygon) {
        if (m_boundary.length() > 1)
            m_boundary << m_boundary.last();
        
        m_boundary << p;
    }
    
    for (auto it = m_boundary.begin(); it < m_boundary.end(); it += 2) {
        if (qAbs(it->x()) == 180.0 && qAbs((it + 1)->x()) == 180.0) {
            it = m_boundary.erase(it, it + 2);
        }
    }
    
    connect(m_fir, &ClientList::clientCountChanged, this, &FirArea::refreshStatus);
    refreshStatus();
}

QRectF FirArea::boundingRect() const
{
    if (m_boundingRect.isNull()) { // find bounding rectangle
        QPointF topLeft = m_polygon.first(), bottomRight = m_polygon.first();
        for (auto p: m_polygon) {
            if (topLeft.y() < p.y())
                topLeft.setY(p.y());
            
            if (bottomRight.y() > p.y())
                bottomRight.setY(p.y());
            
            if (topLeft.x() > p.x())
                topLeft.setX(p.x());
            
            if (bottomRight.x() < p.x())
                bottomRight.setX(p.x());
        }
        
        m_boundingRect = QRectF(topLeft, bottomRight);
    }
    
    return m_boundingRect;
}

bool FirArea::isVisible() const
{
    return m_visible;
}

void FirArea::draw(WorldPainter* painter, MapDrawable::DrawFlags flags) const
{
    painter->save();

    bool staffed = m_fir->hasClientOfType<Atc>();
    
    painter->setBrush(m_fillColor);
    painter->setPen(m_fillColor);
    
    if (staffed)
        painter->drawPolygon(m_polygon);
//    else
//        painter->drawLines(transform.map(m_boundary.cbegin(), m_boundary.cend()));
    
    painter->restore();
    
//    if (flags & Debug) {
//        painter->drawRect(m_boundingRect * transform);
//    }
}

void FirArea::refreshStatus()
{
    auto atcs = m_fir->clients<Atc>();
    int fss = std::count_if(atcs.begin(), atcs.end(), [](auto it) { return it->facility() == Atc::Ctr || it->facility() == Atc::Fss; });
    int uirs = std::count_if(atcs.begin(), atcs.end(), [](auto it) { return it->isUir(); });

    if (fss == 0)
        m_fillColor = QColor(175, 175, 175);
    else if (fss > uirs)
        m_fillColor = QColor(176, 32, 32, 30);
    else if (fss == uirs)
        m_fillColor = QColor(33, 145, 176, 30);
    else
        Q_UNREACHABLE();

    m_visible = fss > 0;
}

}} /* namespace Vatsinator::Gui */
