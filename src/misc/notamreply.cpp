/*
 * notamreply.cpp
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

#include "notamreply.h"

namespace Vatsinator { namespace Misc {

NotamReply::NotamReply(const QString& icao, QObject* parent) :
    QObject(parent),
    m_icao(icao) {}

void NotamReply::addNotam(const Notam& notam)
{
    m_notams.append(notam);
}

void NotamReply::setFinished(bool finished)
{
    if (m_finished != finished) {
        m_finished = finished;
        if (m_finished)
            emit this->finished();
    }
}

}} /* namespace Vatsinator::Misc */
