/*
 * worldviewport.cpp
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

#include "worldviewport.h"
#include <QtCore>

namespace {

QList<QRectF> split(const QRectF& rect)
{
    QList<QRectF> list;
    qreal it = rect.left();
    do {
        qreal mod = std::fmod((it + 180.0), 360.0);
        if (mod < 0)
            mod = 360.0 + mod;

        qreal ax = mod - 180.0;
        qreal bx = qMin(it - mod + 360.0, rect.right());
        it = bx;

        mod = std::fmod((bx + 180.0), 360.0);
        if (mod < 0)
            mod = 360.0 + mod;

        bx = mod - 180.0;
        if (bx <= -180.0)
            bx += 360.0;

        list.append(QRectF(QPointF(ax, rect.top()), QPointF(bx, rect.bottom())));
    } while (it < rect.right());

    return list;
}

}

namespace Vatsinator { namespace Gui {

WorldViewport::WorldViewport(const QRectF& screen) :
        m_rectangles(::split(screen)) {}

}} /* namespace Vatsinator::Gui */
