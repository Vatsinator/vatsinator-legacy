/*
 * worldpainter.h
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

#ifndef GUI_WORLDPAINTER_H
#define GUI_WORLDPAINTER_H

#include "guiexport.h"
#include "core/lonlat.h"
#include "gui/worldtransform.h"
#include <QPainter>

namespace Vatsinator { namespace Gui {

/**
 * \ingroup Gui
 * @{
 *
 * The WorldPainter class extens QPainter functionalities so that it is easy
 * to draw anything in geographic coordinate system.
 */
class __VtrGuiApi__ WorldPainter : public QPainter {
public:
    /**
     * Constructs a WorldPainter that paints on the given \c device.
     */
    explicit WorldPainter(const WorldTransform& transform, QPaintDevice *device);

    /**
     * Draws the given \c image into the given \c rectangle.
     */
    void drawImage(const QRectF& rectangle, const QImage& image);

    /**
     * Draws the \c source portion of the given \c image into the given \c rectangle.
     */
    void drawImage(const QRectF& rectangle, const QImage& image, const QRect& source);

    /**
     * Draws a pixmap at the given position.
     *
     * The \c pixmap is places with its center located at the given \c center
     * position.
     *
     * The pixmap rendering is performed in screen coordinates and is not subject
     * to the current projection.
     */
    void drawPixmap(const Core::LonLat& center, const QPixmap& pixmap);

    /**
     * Draws the given \c pixmap into the given \c rectangle, treating
     * \c rectangle as a geo-coordinate one.
     */
    void drawPixmap(const QRectF& rectangle, const QPixmap& pixmap);

    /**
     * Draws the rectangular portion \c source of the given \c pixmap into
     * the given rectangle, treating \c rectangle as a geo-coordinate one.
     */
    void drawPixmap(const QRectF& rectangle, const QPixmap& pixmap, const QRect& source);

    /**
     * Draws the polygon defined by the given \c points using
     * the given \c fillRule.
     */
    void drawPolygon(const QList<Core::LonLat>& points, Qt::FillRule fillRule = Qt::OddEvenFill);

    /**
     * Draws the polygon defined by the given \c points using
     * the given \c fillRule.
     */
    void drawPolygon(const QVector<Core::LonLat>& points, Qt::FillRule fillRule = Qt::OddEvenFill);

    /**
     * Draws the polyline defined by the coordinate list using the current
     * pen.
     */
    void drawPolyline(const QList<Core::LonLat>& coords);

    /**
     * Draws a line from \c p1 to \c p2.
     */
    void drawLine(const Core::LonLat& p1, const Core::LonLat& p2);

    /**
     * Draws a line for each pair of points in the vector \c pointPairs using the current pen.
     */
    void drawLines(const QVector<Core::LonLat>& pointPairs);

    /**
     * Returns transform used by this painter.
     */
    const WorldTransform& transform() const { return m_transform; }

    using QPainter::drawImage;

private:
    WorldTransform m_transform;

}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_WORLDPAINTER_H
