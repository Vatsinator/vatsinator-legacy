/*
 * tile.h
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

#ifndef TILE_H
#define TILE_H

#include <QObject>
#include <QRectF>
#include <QPixmap>
#include <QMutex>
#include "tileurl.h"

class TileManager;

class Tile : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(bool ready READ isReady NOTIFY ready)
    Q_PROPERTY(quint64 x READ x CONSTANT)
    Q_PROPERTY(quint64 y READ y CONSTANT)
    Q_PROPERTY(quint64 zoom READ zoom CONSTANT)
    
signals:
    void ready();

public:
    explicit Tile(quint64 x, quint64 y, quint64 zoom, TileManager* tm, QObject *parent = nullptr);
    
    const QRectF& coords() const;
    
    QPixmap pixmap(QRect* source) const;
    
    inline bool isReady() const
    {
        return __ready;
    }
    
    inline quint64 x() const
    {
        return __x;
    }
    
    inline quint64 y() const
    {
        return __y;
    }
    
    inline quint64 zoom() const
    {
        return __zoom;
    }
    
    inline const TileUrl& url() const
    {
        return __url;
    }
    
protected:
    void customEvent(QEvent* event) override;
    
private:
    void __fetchTile();
    
private slots:
    void __loadTile();
    
private:
    bool __ready;
    TileManager* __tm;
    const quint64 __x;
    const quint64 __y;
    const quint64 __zoom;
    
    TileUrl __url;
    mutable QRectF __coords;
    
    QPixmap __pixmap;
    mutable QMutex __mutex;
};

#endif // TILE_H
