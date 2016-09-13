/*
 * idldrawer.cpp
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

#include "idldrawer.h"

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;

IdlDrawer::IdlDrawer(QObject *parent) : QObject(parent)
{
    m_idl.append(LonLat(180.0, 85.0511));
    m_idl.append(LonLat(180.0, 75.0));
    m_idl.append(LonLat(-169.25 + 360.0, 67.7356));
    m_idl.append(LonLat(-169.25 + 360.0, 65.0189));
    m_idl.append(LonLat(170.05, 52.6863));
    m_idl.append(LonLat(180.0, 47.8353));
    m_idl.append(LonLat(180.0, -0.9000));
    m_idl.append(LonLat(-159.65 + 360.0, -0.9000));
    m_idl.append(LonLat(-159.65 + 360.0, 2.9));
    m_idl.append(LonLat(-161.85 + 360.0, 2.9));
    m_idl.append(LonLat(-161.85 + 360.0, 5.0));
    m_idl.append(LonLat(-155.95 + 360.0, 5.0));
    m_idl.append(LonLat(-150.65 + 360.0, -7.8));
    m_idl.append(LonLat(-150.65 + 360.0, -10.0));
    m_idl.append(LonLat(-156.05 + 360.0, -10.0));
    m_idl.append(LonLat(-156.05 + 360.0, -7.8));
    m_idl.append(LonLat(-178.05 + 360.0, -7.8));
    m_idl.append(LonLat(-172.75 + 360.0, -15.0));
    m_idl.append(LonLat(-172.75 + 360.0, -45.0));
    m_idl.append(LonLat(180.0, -51.1815));
    m_idl.append(LonLat(180.0, -85.0511));
}

void IdlDrawer::drawBeforeItems(WorldPainter* painter)
{
    painter->save();
    painter->setPen(QPen(QColor(156, 160, 161)));
    painter->drawPolyline(m_idl);
    painter->restore();
}

void IdlDrawer::drawAfterItems(WorldPainter* painter)
{
    Q_UNUSED(painter);
}
