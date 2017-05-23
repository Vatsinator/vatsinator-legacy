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

#include "maprenderer.h"
#include "gui/mapdrawer.h"
#include "gui/mapscene.h"
#include "gui/worldpainter.h"
#include <QtGui>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

MapRenderer::MapRenderer(QObject* parent) :
    QObject(parent)
{
    connect(this, &MapRenderer::centerChanged, &MapRenderer::invalidateTransform);
    connect(this, &MapRenderer::zoomChanged, &MapRenderer::invalidateTransform);
    connect(this, &MapRenderer::viewportChanged, &MapRenderer::invalidateTransform);
    
    connect(this, &MapRenderer::centerChanged, &MapRenderer::updated);
    connect(this, &MapRenderer::zoomChanged, &MapRenderer::updated);
    connect(this, &MapRenderer::viewportChanged, &MapRenderer::updated);
    
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains("VTR_MAP_DEBUG"))
        m_defaultDrawFlags |= MapItem::Debug;
}

MapRenderer::~MapRenderer() {}

const WorldTransform& MapRenderer::transform() const
{
    if (!m_transformValid) {
        m_transform = WorldTransform(viewport(), center(), zoom());
        m_transformValid = true;
    }

    return m_transform;
}

void MapRenderer::setMapDrawer(MapDrawer* drawer)
{
    if (m_mapDrawer)
        delete m_mapDrawer;

    m_mapDrawer = drawer;
}

void MapRenderer::attachMapAddon(MapAddon* addon)
{
    if (!m_addons.contains(addon))
        m_addons.append(addon);
}

void MapRenderer::removeMapAddon(MapAddon* addon)
{
    m_addons.removeAll(addon);
}

void MapRenderer::moveTo(const LonLat& target)
{
    if (m_animation)
        m_animation->stop();
    
    QPropertyAnimation* animation = new QPropertyAnimation(this, "center");
    animation->setEndValue(QVariant::fromValue(target));
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    m_animation = animation;
}

void MapRenderer::zoomTo(qreal targetZoom)
{
    if (m_animation)
        m_animation->stop();
    
    QPropertyAnimation* animation = new QPropertyAnimation(this, "zoom");
    animation->setEndValue(targetZoom);
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::OutCubic));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    m_animation = animation;
}

void MapRenderer::setScene(MapScene* scene)
{
    if (m_scene)
        disconnect(m_scene);
    
    m_scene = scene;
    connect(m_scene, &MapScene::updated, this, &MapRenderer::updated);
    connect(m_scene, &MapScene::trackedFlightChanged, this, &MapRenderer::handleFlightTracked);
}

void MapRenderer::setCenter(const LonLat& center)
{
    if (center != m_center) {
        m_center = center;
        emit centerChanged(m_center);
    }
}

void MapRenderer::setZoom(qreal zoom)
{
    if (zoom < 1.0)
        return;
    
    if (zoom != m_zoom) {
        m_zoom = zoom;
        emit zoomChanged(m_zoom);
    }
}

void MapRenderer::setViewport(const QSize& viewport)
{
    if (viewport != m_viewport) {
        m_viewport = viewport;
        emit viewportChanged(m_viewport);
    }
}

void MapRenderer::update()
{
    emit updated();
}

void MapRenderer::paint(QPaintDevice* device, const QSet<const MapItem*>& selectedItems)
{
    if (!scene()) {
        qWarning("No scene set! Cannot render without a scene. Did you forget to call MapRenderer::setScene()?");
        return;
    }

    MapItem::DrawFlags flags = m_defaultDrawFlags;
    auto transform = this->transform();
    WorldPainter painter(transform, device);
    WorldViewport viewport(transform.worldViewport());

    // Use antyaliasing for nicer lines & polygons
    painter.setRenderHint(QPainter::Antialiasing);
    
    // If we have world map drawer, use it
    if (m_mapDrawer)
        m_mapDrawer->draw(&painter);

    for (MapAddon* addon: this->m_addons)
        addon->drawBeforeItems(&painter);

    // Draw areas, they are below items
    QList<const MapArea*> areas;
    for (const QRectF& rect: qAsConst(viewport.rectangles()))
        areas.append(scene()->areasInRect(rect));

    for (const MapArea* a: areas)
        a->draw(&painter, flags);
    
    // Pick all items that are on the screen
    QList<const MapItem*> items;
    for (const QRectF& rect: qAsConst(viewport.rectangles()))
        items.append(scene()->itemsInRect(rect));

    // Sort all items
    std::sort(items.begin(), items.end(), [](auto a, auto b) {
        return a->z() < b->z();
    });

    for (const MapItem* item: items)
        item->draw(&painter, selectedItems.contains(item) ? (flags | MapItem::DrawSelected) : flags);

    for (MapAddon* addon: this->m_addons)
        addon->drawAfterItems(&painter);
}

void MapRenderer::invalidateTransform()
{
//    WorldTransform transform = this->transform();
//    QRectF screen(transform.map(QPoint(0, 0)), transform.map(QPoint(viewport().width(), viewport().height())));
    m_transformValid = false;
}

void MapRenderer::handleFlightTracked(const Pilot* flight)
{
    if (m_flightTracking)
        disconnect(m_flightTracking);

    if (flight) {
        m_flightTracking = connect(flight, &Pilot::positionChanged, this, &MapRenderer::moveTo);
//        setCenter(flight->position());
        moveTo(flight->position());
    }
}

}} /* namespace Vatsinator::Gui */
