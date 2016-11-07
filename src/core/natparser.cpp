/*
 * natparser.cpp
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

#include "natparser.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

NatParser::NatParser(const QString& filledRoute) : m_filledRoute(filledRoute) {}

NatParser::~NatParser() {}

bool NatParser::natFound()
{
    if (m_waypoints.size() > 0)
        return true;

    return natMethod1() || natMethod2() || natMethod3();
}

bool NatParser::natMethod1()
{
    /* 50N50W 51N40W 52N30W 52N20W */
    static QRegularExpression regex(QStringLiteral("\\s([0-9]{2})([NS])([0-9]{2,3})([EW])\\b"));

    int offset = 0;
    auto match = regex.match(m_filledRoute);
    while (match.lastCapturedIndex() == 4) {
        int ns = match.captured(1).toInt();
        if (match.captured(2) == QStringLiteral("S"))
            ns = -ns;

        int we = match.captured(3).toInt();
        if (match.captured(4) == QStringLiteral("W"))
            we = -we;

        m_waypoints << LonLat(static_cast<qreal>(we), static_cast<qreal>(ns));

        offset = match.capturedEnd(4);
        match = regex.match(m_filledRoute, offset);
    }

    return m_waypoints.size() > 0;
}

bool NatParser::natMethod2()
{
    /* 5000N 05000W 5100N 04000W 5100N 03000W 5100N 02000W */
    static QRegularExpression regex(QStringLiteral("\\b([0-9]{4})([NS])\\s([0-9]{5})([EW])\\b"));

    int offset = 0;
    auto match = regex.match(m_filledRoute);
    while (match.lastCapturedIndex() == 4) {
        int ns = match.captured(1).toInt();
        if (match.captured(2) == QStringLiteral("S"))
            ns = -ns;

        int we = match.captured(3).toInt();
        if (match.captured(4) == QStringLiteral("W"))
            we = -we;

        m_waypoints << LonLat(static_cast<qreal>(we), static_cast<qreal>(ns));

        offset = match.capturedEnd(4);
        match = regex.match(m_filledRoute,  offset);
    }

    return m_waypoints.size() > 0;
}

bool NatParser::natMethod3()
{
    /* 4950N 5140N 5130N 5120N */
    static QRegularExpression regex(QStringLiteral("\\b([0-9]{2})([0-9]{2})([NS])\\b"));

    int offset = 0;
    auto match = regex.match(m_filledRoute);
    while (match.lastCapturedIndex() == 3) {
        int ns = match.captured(1).toInt();
        int we = 0 - match.captured(2).toInt();
        if (match.captured(3) == QStringLiteral("S"))
            ns = -ns;

        m_waypoints << LonLat(static_cast<qreal>(we), static_cast<qreal>(ns));

        offset = match.capturedEnd(3);
        match = regex.match(m_filledRoute, offset);
    }

    return m_waypoints.size() > 0;
}



}} /* namespace Vatsinator::Core */
