/*
 * tileurl.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include <QtCore>

#include "tileurl.h"

TileUrl::TileUrl(quint64 x, quint64 y, quint64 zoom) :
    __x(x),
    __y(y),
    __zoom(zoom)
{

}

QUrl
TileUrl::toUrl() const
{
//     return QUrl(QStringLiteral("http://c.tile.openstreetmap.org/") % path());
    return QUrl(QStringLiteral("http://otile1.mqcdn.com/tiles/1.0.0/sat/") % path());
}

QString
TileUrl::path() const
{
    return QStringLiteral("%1/%2/%3.jpg").arg(
        QString::number(__zoom),
        QString::number(__x),
        QString::number(__y));
}

bool
TileUrl::operator ==(const TileUrl& other) const
{
    return __x == other.__x && __y == other.__y && __zoom == other.__zoom;
}

TileUrl
TileUrl::fromUrl(const QUrl& url)
{
    QRegExp rx("tiles/1\\.0\\.0/sat/(\\d+)/(\\d+)/(\\d+).{3}");
    Q_ASSERT(rx.indexIn(url.path()) != -1);
    
    quint64 x = rx.cap(2).toULongLong();
    quint64 y = rx.cap(3).toULongLong();
    quint64 zoom = rx.cap(1).toULongLong();
    
    return TileUrl(x, y, zoom);
}
