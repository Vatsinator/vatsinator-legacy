/*
 * maprenderer.cpp
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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
#include <chrono>

#include "plugins/mapdrawer.h"
#include "storage/settingsmanager.h"
#include "ui/map/airportitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/mapscene.h"
#include "ui/map/uiritem.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "maprenderer.h"

namespace {
    inline qreal toMercator(qreal lat)
    {
        return qRadiansToDegrees(qLn(qTan(M_PI / 4 + qDegreesToRadians(lat) / 2)));
    }
    
    inline qreal fromMercator(qreal y)
    {
        return qRadiansToDegrees(2 * qAtan(qExp(qDegreesToRadians(y))) - M_PI / 2);
    }
}

MapRenderer::MapRenderer(QObject* parent) :
    QObject(parent),
    __mapDrawer(nullptr),
    __scene(new MapScene(this))
{
    __restoreSettings();
}

MapRenderer::~MapRenderer()
{
    if (__mapDrawer)
        delete __mapDrawer;
    
    __storeSettings();
}

LonLat
MapRenderer::mapToLonLat(const QPoint& point)
{
    qreal x = static_cast<qreal>(point.x() - (__viewport.width() / 2)) * MapConfig::longitudeMax() / __viewport.width() / zoom() + center().x();
    qreal y = static_cast<qreal>(point.y() - (__viewport.height() / 2)) * MapConfig::latitudeMax() / __viewport.height() / zoom() - center().y();
    return LonLat(x, -::fromMercator(y));
}

LonLat
MapRenderer::scaleToLonLat(const QPoint& point)
{
    qreal x = static_cast<qreal>(point.x()) * MapConfig::longitudeMax() / __viewport.width() / zoom();
    qreal y = static_cast<qreal>(point.y()) * MapConfig::latitudeMax() / __viewport.height() / zoom();
    return LonLat(x, ::fromMercator(y));
}

QPoint
MapRenderer::mapFromLonLat(const LonLat& point)
{
    int x = (__viewport.width() * (2.0 * zoom() * (point.x() - center().x()) + MapConfig::longitudeMax())) / (2 * MapConfig::longitudeMax());
    int y = (__viewport.height() * (2.0 * zoom() * (-::toMercator(point.y()) + center().y()) + MapConfig::latitudeMax())) / (2 * MapConfig::latitudeMax());
    return QPoint(x, y);
}

void
MapRenderer::setMapDrawer(MapDrawer* drawer)
{
    __mapDrawer = drawer;
    __mapDrawer->initialize(this);
}

void
MapRenderer::setZoom(qreal zoom)
{
    __zoom = zoom;
    __updateScreen();
    emit updated();
    emit zoomChanged(__zoom);
}

void
MapRenderer::setCenter(const LonLat& center)
{
    __center = center;
    __updateScreen();
    emit updated();
    emit centerChanged(__center);
}

qreal
MapRenderer::zoomStep(int steps)
{
    //count limiter for this function
    __actualZoomMaximum =
        qFloor(
            qLn(
                (
                    MapConfig::zoomMaximum() - MapConfig::zoomMinimum()
                ) /
                MapConfig::zoomNormalizeCoef()
            ) / qLn(
                MapConfig::zoomBase() +
                (
                    __scene->settings().misc.zoom_coefficient * 0.01
                )
            )
        );
        
    //set the actual zoom level according to number of scroll wheel steps
    __actualZoom += steps;
    
    //limiting range of zoom
    __actualZoom = qBound(0, __actualZoom, __actualZoomMaximum);
    
    // count value of closeup
    return
        MapConfig::zoomMinimum() + MapConfig::zoomNormalizeCoef() *
        qPow(
            MapConfig::zoomBase() +
            (
                __scene->settings().misc.zoom_coefficient * 0.01
            ),
            __actualZoom
        );
}

void
MapRenderer::setViewport(const QSize& size)
{
    __viewport = size;
    __updateScreen();
    emit updated();
    emit viewportChanged(__viewport);
}

QPointF
MapRenderer::toMercator(const LonLat& lonLat)
{
    return QPointF(lonLat.x(), qRadiansToDegrees(qLn(qTan(M_PI / 4 + qDegreesToRadians(lonLat.y()) / 2))));
}

void
MapRenderer::paint(QPainter* painter)
{
    QTransform transform;
    transform.translate(__viewport.width() / 2, __viewport.height() / 2);
    transform.scale(__viewport.width() / MapConfig::longitudeMax(), -__viewport.height() / MapConfig::latitudeMax());
    transform.scale(zoom(), zoom());
    transform.translate(-center().x(), -center().y());
    
    if (__mapDrawer) {
        __mapDrawer->draw(painter, transform);
    }
    
    scene()->inRect(__screen, [painter, &transform](const MapItem* item) {
        item->draw(painter, transform);
    });
}

void
MapRenderer::__storeSettings()
{
    QSettings settings;
    
    settings.beginGroup("CameraSettings");
    
    settings.setValue("actualZoomCoefficient", __actualZoom);
    settings.setValue("zoom", __zoom);
    settings.setValue("center", QVariant::fromValue<LonLat>(__center));
    
    settings.endGroup();
}

void
MapRenderer::__restoreSettings()
{
    QSettings settings;
    
    settings.beginGroup("CameraSettings");
    
    __actualZoom = settings.value("actualZoomCoefficient", 0).toInt();
    __zoom = settings.value("zoom", MapConfig::zoomDefault()).toReal();
    __center = settings.value("center", QVariant::fromValue<LonLat>(LonLat(0.0, 0.0))).value<LonLat>();
    
    settings.endGroup();
}

void
MapRenderer::__updateScreen()
{
    __screen.setTopLeft(mapToLonLat(QPoint(0, 0)));
    __screen.setBottomRight(mapToLonLat(QPoint(__viewport.width(), __viewport.height())));
}