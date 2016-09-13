/*
 * worldtransform.cpp
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

#include "worldtransform.h"
#include <QtCore>

using namespace Vatsinator::Core;

namespace {

qreal toMercator(qreal lat)
{
    return qRadiansToDegrees(qLn(qTan(M_PI / 4 + qDegreesToRadians(lat) / 2)));
}

qreal fromMercator(qreal y)
{
    return qRadiansToDegrees(2 * qAtan(qExp(qDegreesToRadians(y))) - M_PI / 2);
}

} // ns

namespace Vatsinator { namespace Gui {

WorldTransform::WorldTransform(const QSize& viewport, const LonLat& offset,
                               qreal scale) :
    m_viewport(viewport),
    m_mFactor(qMax(viewport.width(), viewport.height())),
    m_offset(offset),
    m_scale(scale),
    m_mapped360(m_mFactor * scale) {}

QPoint WorldTransform::map(const LonLat& lonLat, bool* visible) const
{
    bool v = true;
    
    int x = static_cast<int>(mapLongitude(lonLat.longitude()));
    if (x < 0) {
        int x2 = x + m_mapped360;
        if (x2 < 0)
            v = false;
        else
            x = x2;
    } else if (x > viewport().width()) {
        int x2 = x - m_mapped360;
        if (x2 > viewport().width())
            v = false;
        else
            x = x2;
    }

    int y = static_cast<int>(mapLatitude(lonLat.latitude()));

    if (visible)
        *visible = v;
    
    return QPoint(x, y);
}

QRect WorldTransform::map(const QRectF& rect, bool* visible) const
{
    bool v = true;

    int left = static_cast<int>(mapLongitude(rect.left()));
    int right = static_cast<int>(mapLongitude(rect.right()));
    if (right < 0) {
        int right2 = right + m_mapped360;
        if (right2 < 0) {
            v = false;
        } else {
            right = right2;
            left += m_mapped360;
        }
    } else if (left > viewport().width()) {
        int left2 = left - m_mapped360;
        if (left2 > viewport().width()) {
            v = false;
        } else {
            left = left2;
            right -= m_mapped360;
        }
    }

    int top = static_cast<int>(mapLatitude(rect.top()));
    int bottom = static_cast<int>(mapLatitude(rect.bottom()));

    if (visible)
        *visible = v;

    return QRect(QPoint(left, top), QPoint(right, bottom));
}

QPointF WorldTransform::mapF(const LonLat& lonLat, bool* visible) const
{
    qreal x = mapLongitude(lonLat.longitude());
    qreal y = mapLatitude(lonLat.latitude());

    if (visible)
        *visible = y >= 0.0 && y <= viewport().height() && x >= 0.0 && x <= viewport().width();
    
    return QPointF(x, y);
}

LonLat WorldTransform::map(const QPoint& point) const
{
    qreal x = (point.x() * 360.0 - 180.0 * viewport().width()) / (m_mFactor * scale()) + offset().longitude();
    qreal y = ::fromMercator((180.0 * viewport().height() - point.y() * 360.0) / (m_mFactor * scale()) + ::toMercator(offset().latitude()));

    return LonLat(x, y);
}

LonLat WorldTransform::mapF(const QPointF& point) const
{
    qreal x = (point.x() * 360.0 - 180.0 * viewport().width()) / (m_mFactor * scale()) + offset().longitude();
    qreal y = ::fromMercator((180.0 * viewport().height() - point.y() * 360.0) / (m_mFactor * scale()) + ::toMercator(offset().latitude()));

    return LonLat(x, y);
}

WorldViewport WorldTransform::worldViewport() const
{
    QRectF screen(map(QPoint(0, 0)), map(QPoint(viewport().width(), viewport().height())));
    return WorldViewport(screen);
}

qreal WorldTransform::mapLongitude(qreal longitude) const
{
    return (longitude - offset().longitude()) * m_mFactor * scale() / 360.0 + static_cast<qreal>(viewport().width()) / 2.0;
}

qreal WorldTransform::mapLatitude(qreal latitude) const
{
    return (-::toMercator(latitude) + ::toMercator(offset().latitude())) * m_mFactor * scale() / 360.0 + static_cast<qreal>(viewport().height()) / 2.0;
}


}} /* namespace Vatsinator::Gui */
