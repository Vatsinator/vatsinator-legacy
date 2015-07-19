/*
 * tileurl.h
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

#ifndef TILEURL_H
#define TILEURL_H

#include <QtGlobal>
#include <QUrl>
#include <QString>

class TileUrl {
    
public:
    TileUrl();
    explicit TileUrl(quint64 x, quint64 y, quint64 zoom);
    
    QUrl toUrl() const;
    QString path() const;
    bool isValid() const;
    
    bool operator ==(const TileUrl& other) const;
    
    inline quint64 zoom() const
    {
        return __zoom;
    }
    
    static TileUrl fromUrl(const QUrl& url);
    
private:
    quint64 __x;
    quint64 __y;
    quint64 __zoom;

};

#endif // TILEURL_H
