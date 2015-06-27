/*
 * map.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include <QtQuick>

#include "storage/filemanager.h"
#include "ui/map/maprenderer.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "map.h"

Map::Map(QQuickItem* parent) :
    QQuickItem(parent),
    __renderer(nullptr)
{
    setFlag(QQuickItem::ItemHasContents);
    
    connect(this, &Map::windowChanged, this, &Map::__handleWindowChange);
    connect(qApp, &QGuiApplication::applicationStateChanged,
            this, &Map::__handleApplicationStateChange,
            Qt::DirectConnection);
}

Map::~Map()
{

}

QImage
Map::grab()
{
    if (__cached.isNull())
        cache();
    
    return __cached;
}

void
Map::updateZoom(qreal factor)
{
    if (__renderer)
        __renderer->setZoom(__renderer->zoom() + (__renderer->zoom() * factor));
        
    if (window())
        window()->update();
}

void
Map::updatePosition(int x, int y)
{
    if (__renderer) {
        LonLat center = __renderer->center();
        QPoint p(x, -y);
        center -= __renderer->scaleToLonLat(p);
        center.ry() = qBound(-90.0, center.y(), 90.0);
        
        if (center.x() < -180.0)
            center.rx() += 360.0;
            
        if (center.x() > 180.0)
            center.rx() -= 360.0;
            
        __renderer->setCenter(center);
    }
    
    if (window())
        window()->update();
}

QString
Map::cachedImageSource() const
{
    return QDir::cleanPath(
               QStandardPaths::writableLocation(QStandardPaths::CacheLocation) %
               QDir::separator() %
               QStringLiteral("Vatsinator") %
               QDir::separator()
           ) % QStringLiteral("mapimage.png");
}

void
Map::sync()
{
    if (!__renderer) {
        __renderer = new MapRenderer();
        connect(window(),   SIGNAL(beforeRendering()),
                __renderer, SLOT(paint()),  Qt::DirectConnection);
        emit ready();
    }
    
    __renderer->setViewport(window()->size() * window()->devicePixelRatio());
}

void
Map::cleanup()
{
    if (__renderer) {
        delete __renderer;
        __renderer = nullptr;
    }
}

void
Map::cache()
{
    if (window()) {
        qDebug("Caching map...");
        __cached = window()->grabWindow();
    }
}

void
Map::__cacheMapImage()
{
    if (!window()) {
        qWarning("No window, can't cache the map image");
        return;
    }
    
    QImage image = grab();
    bool result = image.save(cachedImageSource());
    
    if (!result)
        qWarning("Could not save the map image");
}

void
Map::__handleWindowChange(QQuickWindow* window)
{
    if (window) {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &Map::sync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &Map::cleanup, Qt::DirectConnection);
        connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimDataUpdated, window, &QQuickWindow::update);
        window->setClearBeforeRendering(false);
    }
}

void
Map::__handleApplicationStateChange(Qt::ApplicationState state)
{
    switch (state) {
        case Qt::ApplicationSuspended:
        case Qt::ApplicationHidden:
        case Qt::ApplicationInactive:
            qDebug("Deactivating map...");
            __cacheMapImage();
            break;
            
        case Qt::ApplicationActive:
            qDebug("Restoring map...");
            /* TODO */
            break;
    }
}
