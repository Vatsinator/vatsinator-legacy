/*
 * worldtransform.h
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

#ifndef GUI_WORLDTRANSFORM_H
#define GUI_WORLDTRANSFORM_H

#include "core/lonlat.h"
#include "gui/worldviewport.h"
#include "gui/v8rgui_export.h"
#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QSize>
#include <QtCore/QVector>

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 * 
 * The WorldTransform class represents a transformation between
 * global coordinate system (longitude/latitude) and widget one, used
 * by the QPainter.
 */
class V8RGUI_EXPORT WorldTransform {
public:
    /**
     * The constructor that takes the \c viewport size, the mapping \c offset
     * and the \c scale of the transform.
     */
    explicit WorldTransform(const QSize& viewport, const Core::LonLat& offset,
                            qreal scale);
    
    /**
     * Maps the given geo position to the widget coordinates.
     */
    QPoint map(const Core::LonLat& lonLat, bool* visible = nullptr) const;

    /**
     * Maps the given rectangle to the widget coordinates.
     */
    QRect map(const QRectF& rect, bool* visible = nullptr) const;
    
    /**
     * Maps the given geo position to screen coordinates, but keeps the
     * precision.
     */
    QPointF mapF(const Core::LonLat& lonLat, bool* visible = nullptr) const;
    
    /**
     * Maps the given widget point to the geo position.
     */
    Core::LonLat map(const QPoint& point) const;
    
    /**
     * Maps the given widget point to the geo position.
     */
    Core::LonLat mapF(const QPointF& point) const;
    
    /**
     * Map the range of points, from \c first to \c last.
     */
    template <typename Iterator>
    QVector<QPoint> map(Iterator first, Iterator last) const;
    
    /**
     * The screen viewport, in pixels.
     */
    const QSize& viewport() const { return m_viewport; }

    /**
     * The transform offset (i.e. the center point).
     */
    const Core::LonLat& offset() const { return m_offset; }

    /**
     * Zoom factor.
     */
    qreal scale() const { return m_scale; }

    WorldViewport worldViewport() const;
    
    WorldTransform() = default;
    WorldTransform(const WorldTransform& other) = default;
    
private:
    qreal mapLongitude(qreal longitude) const;
    qreal mapLatitude(qreal latitude) const;
    
    QSize m_viewport;
    int m_mFactor;
    Core::LonLat m_offset;
    qreal m_scale = 0.0;
    int m_mapped360;
    
}; /** @} */

template <typename Iterator>
QVector<QPoint> WorldTransform::map(Iterator first, Iterator last) const
{
    QVector<QPoint> mapped;
    mapped.reserve(last - first);
    for (auto it = first; it != last; ++it)
        mapped << QPoint(mapLongitude(it->x()), mapLatitude(it->y()));

    bool v = std::any_of(mapped.cbegin(), mapped.cend(), [this](auto it) {
        return it.x() >= 0 && it.x() <= this->viewport().width();
    });

    if (!v) {
        if (mapped.at(0).x() < 0)
            std::transform(mapped.begin(), mapped.end(), mapped.begin(),
                           [this](const QPoint& p) { return QPoint(p.x() + this->m_mapped360, p.y()); });
        else if (mapped.at(0).x() > viewport().width())
            std::transform(mapped.begin(), mapped.end(), mapped.begin(),
                           [this](const QPoint& p) { return QPoint(p.x() - this->m_mapped360, p.y()); });
    }

    return mapped;
}

}} /* namespace Vatsinator::Gui */

#endif // GUI_WORLDTRANSFORM_H
