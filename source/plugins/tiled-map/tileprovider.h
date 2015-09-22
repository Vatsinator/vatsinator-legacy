/*
 * tileprovider.h
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

#ifndef TILEPROVIDER_H
#define TILEPROVIDER_H

#include <QString>

/**
 * The TileProvider class identifies the remote location where all the
 * tiles will be fetched from.
 */
class TileProvider {
public:
    TileProvider(const QString& name, const QString& url, const QString& path);
    
    QUrl url(quint64 x, quint64 y, quint64 zoom);
    
    inline const QString& name() const
    {
        return __name;
    }
    
private:
    QString __name;
    QString __url;
    QString __path;
    
};

#endif // TILEPROVIDER_H
