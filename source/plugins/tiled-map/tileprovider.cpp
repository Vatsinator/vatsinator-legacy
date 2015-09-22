/*
 * tileprovider.cpp
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

#include "tileprovider.h"

namespace {
    QString randomizePrefix() {
        static QVector<QString> prefixes = { "a", "b", "c", "d", "e", "f", "g",
                                             "h", "i", "j", "k", "l", "m", "n",
                                             "o", "p", "q", "r", "s", "t", "u",
                                             "v", "w", "x", "y", "z" };
        return prefixes[qrand() % prefixes.size()];
    }
}

TileProvider::TileProvider(const QString& name, const QString& url, const QString& path) :
    __name(name),
    __url(url),
    __path(path)
{

}

QUrl
TileProvider::url(quint64 x, quint64 y, quint64 zoom)
{
//     return QUrl(QStringLiteral("http://%1.basemaps.cartocdn.com/%2/%3").arg(randomizePrefix(), CartoDBMapPath, path()));
    return QUrl(QString(__url + "/" + __path)
        .replace("<prefix>", randomizePrefix())
        .replace("<x>", QString::number(x))
        .replace("<y>", QString::number(y))
        .replace("<zoom>", QString::number(zoom))
    );
}
