/*
 * tiledmapdrawer.cpp
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

#include <QtGui>

#include "storage/settingsmanager.h"
#include "ui/map/maprenderer.h"
#include "vatsinatorapplication.h"

#include "tile.h"
#include "tilemanager.h"
#include "tilereadyevent.h"

#include "tiledmapdrawer.h"

TiledMapDrawer::TiledMapDrawer(QObject* parent) :
    QObject(parent),
    __tiles(nullptr),
    __providers({
        TileProvider("CartoDB", "http://<prefix>.basemaps.cartocdn.com", "light_nolabels/<zoom>/<x>/<y>.png"), // light
        TileProvider("CartoDB", "http://<prefix>.basemaps.cartocdn.com", "dark_nolabels/<zoom>/<x>/<y>.png") }) // dark
{
    
}

TiledMapDrawer::~TiledMapDrawer()
{
    __tiles->deleteLater();
}

QStringList
TiledMapDrawer::types() const
{
    return { tr("Light"), tr("Dark") };
}

void
TiledMapDrawer::initialize(MapRenderer* renderer)
{
    __tiles = new TileManager(renderer);
    __updateMapType();
    __tiles->initialize();
    
    __renderer = renderer;
    
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged, this, &TiledMapDrawer::__updateMapType);
}

void
TiledMapDrawer::draw(QPainter* painter, const WorldTransform& transform)
{
    Q_ASSERT(__tiles);
    
#ifndef Q_OS_ANDROID
    /* Don't enable smooth pixmap transform on Android for better performance */
    auto hints = painter->renderHints();
    painter->setRenderHints(hints | QPainter::SmoothPixmapTransform);
#endif
    
    auto tiles = __tiles->tiles();
    std::for_each(tiles.begin(), tiles.end(), [&transform, painter](Tile* tile) {
        QRect coords = tile->coords() * transform;
        QRect source;
        QPixmap px = tile->pixmap(&source);
        painter->drawPixmap(coords, px, source);
        
#ifndef QT_NO_DEBUG
        /* Extra info */
        painter->drawText(coords, Qt::AlignTop | Qt::AlignLeft, QStringLiteral("%1/%2/%3").arg(
            QString::number(tile->zoom()),
            QString::number(tile->x()),
            QString::number(tile->y())));
        painter->drawRect(coords);
#endif
    });

#ifndef Q_OS_ANDROID
    painter->setRenderHints(hints);
#endif
}


void
TiledMapDrawer::__updateMapType()
{
    int selectedType = SM::get("map.map_type").toInt();
    if (selectedType < 0) // ui not yet initialized
        return;
    
    qDebug() << "Selected map type: " << types()[selectedType];
    
    __tiles->setTileProvider(&__providers[selectedType]);
}
