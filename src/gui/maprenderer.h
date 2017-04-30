/*
 * maprenderer.h
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

#ifndef GUI_MAPRENDERER_H
#define GUI_MAPRENDERER_H

#include "core/lonlat.h"
#include "core/pilot.h"
#include "gui/mapaddon.h"
#include "gui/mapitem.h"
#include "gui/worldtransform.h"
#include "gui/vtrgui_export.h"
#include <QtCore/QAbstractAnimation>
#include <QtCore/QMap>
#include <QtCore/QMetaObject>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtCore/QSize>
#include <QtGui/QPaintDevice>

namespace Vatsinator { namespace Gui {

class MapDrawer;
class MapScene;

/**
 * \ingroup Gui
 * @{
 * 
 * The MapRenderer class takes care of rendering the map.
 *
 * \sa MapScene.
 */
class VTRGUI_EXPORT MapRenderer : public QObject {
    Q_OBJECT
    
    /**
     * The \c scene property keeps the scene that is rendered.
     */
    Q_PROPERTY(MapScene* scene READ scene WRITE setScene)
    
    /**
     * The center property represents the center of the map.
     */
    Q_PROPERTY(Vatsinator::Core::LonLat center READ center WRITE setCenter NOTIFY centerChanged)
    
    /**
     * The zoom property describes the current zoom of the map. The larger the
     * value is, the smaller range is visible.
     * 
     * \note The \c MapRenderer::setZoom() method will never accept values lower than 1.0.
     */
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    
    /**
     * Size of the screen (in pixels) on which the map is being rendered.
     */
    Q_PROPERTY(QSize viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    
signals:
    /**
     * One or more of the properties have changed and therefore the map needs
     * to be re-rendered.
     */
    void updated();
    
    void centerChanged(const Vatsinator::Core::LonLat& center);
    void zoomChanged(qreal zoom);
    void viewportChanged(const QSize& viewport);

public:
    /**
     * The default constructor passes \c parent to the QObject's constructor.
     */
    explicit MapRenderer(QObject* parent = nullptr);
    
    /**
     * Destroys the MapRenderer.
     */
    virtual ~MapRenderer();
    
    /**
     * Returns the current transform.
     */
    const WorldTransform& transform() const;
    
    /**
     * Sets the MapDrawer implementation.
     */
    void setMapDrawer(MapDrawer* drawer);

    /**
     * Adds the addon to the rendering pipeline.
     */
    void attachMapAddon(MapAddon* addon);
    
    /**
     * Moves the \c center of the map to the given point asynchronously.
     */
    void moveTo(const Core::LonLat& target);
    
    /**
     * Smoothly zooms the map to the value specified.
     */
    void zoomTo(qreal targetZoom);
    
    /**
     * Gives direct access to the currently running MapDrawer implementation.
     * @{
     */
    MapDrawer* mapDrawer() { return m_mapDrawer; }
    const MapDrawer* mapDrawer() const { return m_mapDrawer; }
    /** @} */
    
    const MapScene* scene() const { return m_scene; }
    MapScene* scene() { return m_scene; }
    void setScene(MapScene* scene);
    
    const Core::LonLat& center() const { return m_center; }
    void setCenter(const Core::LonLat& center);
    
    qreal zoom() const { return m_zoom; }
    void setZoom(qreal zoom);
    
    const QSize& viewport() const { return m_viewport; }
    void setViewport(const QSize& viewport);

public slots:
    /**
     * Requests a repaint.
     */
    void update();
    
    /**
     * Renders the scene on the given device.
     */
    virtual void paint(QPaintDevice* device, const QSet<const MapItem*>& selectedItems = QSet<const MapItem*>());
    
private slots:
    /**
     * Splits the screen to transforms as required.
     */
    void invalidateTransform();

    /**
     * Handles flight tracking in the scene.
     */
    void handleFlightTracked(const Core::Pilot* flight);
    
private:
    MapScene* m_scene = nullptr;
    Core::LonLat m_center;
    qreal m_zoom = 1.0; /**< The default value of 1.0 will show the whole world map in the viewport */
    QSize m_viewport;
    mutable WorldTransform m_transform;
    mutable bool m_transformValid = false;
    MapItem::DrawFlags m_defaultDrawFlags = 0;
    MapDrawer* m_mapDrawer = nullptr;
    QPointer<QAbstractAnimation> m_animation;
    QMetaObject::Connection m_flightTracking; /**< A connection for tracking the flight */
    QList<MapAddon*> m_addons;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_MAPRENDERER_H
