/*
 * worldpainter.cpp
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#include "worldpainter.h"
#include <QtGui>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

WorldPainter::WorldPainter(const WorldTransform& transform, QPaintDevice* device) :
        QPainter(device),
        m_transform(transform) {}

void WorldPainter::drawPixmap(const LonLat& center, const QPixmap& pixmap)
{
    bool visible;
    QPoint mapped = m_transform.map(center, &visible);
    if (visible) {
        QSize size = pixmap.size() / qApp->primaryScreen()->devicePixelRatio();
        QPainter::drawPixmap(mapped.x() - size.width() / 2, mapped.y() - size.height() / 2, pixmap);
    }
}

void WorldPainter::drawPixmap(const QRectF& rectangle, const QPixmap& pixmap)
{
    QRect rect(m_transform.map(rectangle));
    QPainter::drawPixmap(rect, pixmap);
}

void WorldPainter::drawPixmap(const QRectF& rectangle, const QPixmap& pixmap, const QRect& source)
{
    QPainter::drawPixmap(m_transform.map(rectangle), pixmap, source);
}

void WorldPainter::drawPolygon(const QList<LonLat>& points, Qt::FillRule fillRule)
{
    QPainter::drawPolygon(m_transform.map(points.cbegin(), points.cend()), fillRule);
}

void WorldPainter::drawPolygon(const QVector<LonLat>& points, Qt::FillRule fillRule)
{
    QPainter::drawPolygon(m_transform.map(points.cbegin(), points.cend()), fillRule);
}

void WorldPainter::drawPolyline(const QList<LonLat>& coords)
{
    QVector<QPoint> points = m_transform.map(coords.cbegin(), coords.cend());
    QPainter::drawPolyline(points.constData(), points.count());
}

void WorldPainter::drawLine(const LonLat& p1, const LonLat& p2)
{
    QPoint m1 = m_transform.map(p1);
    QPoint m2 = m_transform.map(p2);
    QPainter::drawLine(m1, m2);
}

void WorldPainter::drawLines(const QVector<LonLat>& pointPairs)
{
    QVector<QPoint> points = m_transform.map(pointPairs.cbegin(), pointPairs.cend());
    QPainter::drawLines(points);
}

}} /* namespace Vatsinator::Gui */
