/*
 * maprenderer.cpp
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

#include <QtGui>
#include <chrono>
#include <cmath>

#include "plugins/mapdrawer.h"
#include "storage/settingsmanager.h"
#include "ui/map/maparea.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/mapscene.h"
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
    
    /**
     * Splits the given \c screen to a valid transforms.
     */
    QList<WorldTransform>
    splitScreenToTransforms(const QSize& viewport, const LonLat& offset, qreal scale, const QRectF& screen)
    {
        QList<WorldTransform> list;
        qreal it = screen.left();
        do {
            qreal tmp = it;
            qreal mod = std::fmod((it + 180.0), 360.0);
            if (mod < 0)
                mod = 360.0 + mod;
            
            qreal ax = mod - 180.0;
            qreal bx = qMin(it - mod + 360.0, screen.right());
            it = bx;
            
            mod = std::fmod((bx + 180.0), 360.0);
            if (mod < 0)
                mod = 360.0 + mod;
            
            bx = mod - 180.0;
            if (bx <= -180.0)
                bx += 360.0;
            
            QRectF trScreen(QPointF(ax, screen.top()), QPointF(bx, screen.bottom()));
            LonLat trOffset = offset;
            qreal m = qRound((it + tmp) / 2 / 360.0) * -360.0;
            trOffset.setX(trOffset.x() + m);
            
            list << WorldTransform(viewport, trOffset, scale, trScreen);
        } while (it < screen.right());
        
        return list;
    }
    
}

MapRenderer::MapRenderer(QObject* parent) :
    QObject(parent),
    __isRendering(false),
    __mapDrawer(nullptr),
    __scene(new MapScene(this))
{
    __restoreMapState();
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimDataUpdated, this, &MapRenderer::updated);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &MapRenderer::__saveMapState);
}

MapRenderer::~MapRenderer()
{
    if (__mapDrawer)
        delete __mapDrawer;
}

LonLat
MapRenderer::mapToLonLat(const QPoint& point)
{
    qreal m = qMax(viewport().width(), viewport().height());
    
    qreal x = (point.x() * 360 - 180 * viewport().width()) / (m * zoom()) + center().longitude();
    qreal y = fromMercator((180 * viewport().height() - point.y() * 360) / (m * zoom()) + toMercator(center().latitude()));
    
    return LonLat(x, y);
}

LonLat
MapRenderer::mapToLonLat(const QPointF& point)
{
    qreal m = qMax(viewport().width(), viewport().height());
    
    qreal x = (point.x() * 360.0 - 180.0 * viewport().width()) / (m * zoom()) + center().longitude();
    qreal y = fromMercator((180 * viewport().height() - point.y() * 360) / (m * zoom()) + toMercator(center().latitude()));
    
    return LonLat(x, y);
}

void
MapRenderer::setMapDrawer(MapDrawer* drawer)
{
    if (__mapDrawer)
        delete __mapDrawer;
    
    __mapDrawer = drawer;
    __mapDrawer->initialize(this);
    
    emit mapDrawerChanged(__mapDrawer);
}

void
MapRenderer::setZoom(qreal zoom)
{
    __zoom = zoom;
    __updateTransforms();
    emit updated();
    
    if (!scene()->isAnimating())
        emit zoomChanged(__zoom);
}

void
MapRenderer::setCenter(const LonLat& center)
{
    __center = center;
    __updateTransforms();
    emit updated();
    emit centerChanged(__center);
}

qreal
MapRenderer::zoomStep(int steps)
{
    //count limiter for this function
    __actualZoomMaximum =
        qFloor(qLn((MapConfig::zoomMaximum() - MapConfig::zoomMinimum()) / MapConfig::zoomNormalizeCoef()) / qLn(MapConfig::zoomBase() + (MapConfig::zoomCoefficient() * 0.01)));
    
    //set the actual zoom level according to number of scroll wheel steps
    __actualZoom += steps;
    
    //limiting range of zoom
    __actualZoom = qBound(0, __actualZoom, __actualZoomMaximum);
    
    // count value of closeup
    return MapConfig::zoomMinimum() + MapConfig::zoomNormalizeCoef() *
        qPow(MapConfig::zoomBase() + (MapConfig::zoomCoefficient() * 0.01), __actualZoom);
}

void
MapRenderer::setViewport(const QSize& size)
{
    __viewport = size;
    __updateTransforms();
    emit updated();
    emit viewportChanged(__viewport);
}

WorldTransform
MapRenderer::transform(qreal longitude) const
{
    int index = static_cast<int>(longitude / 180.0);
    return __transforms[index];
}

void
MapRenderer::paint(QPainter* painter, const QSet<MapItem*>& selectedItems)
{
    __selectedItems = selectedItems;
    __isRendering = true;
    
    auto transforms = __transforms.values();
    std::for_each(transforms.begin(), transforms.end(), [&](auto transform) {
        if (__mapDrawer) {
            __mapDrawer->draw(painter, transform);
        }
        
        this->scene()->inRect(transform.screen(), [painter, &transform](const MapArea* area) {
            area->draw(painter, transform);
        });
        
    });
    
    std::for_each(transforms.begin(), transforms.end(), [&](auto transform) {
        auto items = this->scene()->itemsInRect(transform.screen());
        std::sort(items.begin(), items.end(), [](auto a, auto b) {
            return a->z() < b->z();
        });

        std::for_each(items.begin(), items.end(), [painter, &transform, &selectedItems](auto item) {
            item->draw(painter, transform, selectedItems.contains((MapItem*&)item) ? MapItem::DrawSelected : static_cast<MapItem::DrawFlags>(0));
        });
    });
    
    __isRendering = false;
}

void
MapRenderer::__restoreMapState()
{
    QSettings settings;
    
    settings.beginGroup("MapState");
    
    __actualZoom = settings.value("actualZoomCoefficient", 0).toInt();
    __zoom = settings.value("zoom", MapConfig::zoomDefault()).toReal();
    __center = settings.value("center", QVariant::fromValue<LonLat>(LonLat(0.0, 0.0))).value<LonLat>();
    
    settings.endGroup();
}

void
MapRenderer::__updateTransforms()
{
    __screen.setTopLeft(mapToLonLat(QPoint(0, 0)));
    __screen.setBottomRight(mapToLonLat(QPoint(__viewport.width(), __viewport.height())));
    
    QMap<int, WorldTransform> map;
    auto transforms = splitScreenToTransforms(viewport(), center(), zoom(), screen());
    std::for_each(transforms.begin(), transforms.end(), [&map](auto t) {
        int id = static_cast<int>(t.offset().x() / -180.0);
        map[id] = t;
    });
    
    __transforms.swap(map);
}

void
MapRenderer::__saveMapState()
{
    QSettings settings;
    
    settings.beginGroup("MapState");
    
    settings.setValue("actualZoomCoefficient", __actualZoom);
    settings.setValue("zoom", __zoom);
    settings.setValue("center", QVariant::fromValue<LonLat>(__center));
    
    settings.endGroup();
}
