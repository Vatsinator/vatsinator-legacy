/*
 * maprenderer.h
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

#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include <QObject>
#include <QSize>
#include <QColor>
#include <QRectF>

#include "ui/map/worldtransform.h"
#include "vatsimdata/lonlat.h"

class IconKeeper;
class MapDrawer;
class MapItem;
class MapScene;
class ModelMatcher;
class QPainter;

/**
 * The MapRenderer class takes care of rendering the map.
 *
 * \sa MapItem and MapScene.
 */
class MapRenderer : public QObject {
    Q_OBJECT
    
    /**
     * The zoom property describes the current zoom of the map. The larger the
     * value is, the smaller range is visible.
     */
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    
    /**
     * The center property represents the center of the map.
     */
    Q_PROPERTY(LonLat center READ center WRITE setCenter NOTIFY centerChanged)
    
    /**
     * Size of the screen (in pixels) on which the map is being rendered.
     */
    Q_PROPERTY(QSize viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    
signals:
    /**
     * One or more of the properties have changed and therefore the map needs
     * to be re-rendered.
     *
     * \note Connect to this signal using Qt::DirectConnection to keep
     * it efficient.
     */
    void updated();
    
    void zoomChanged(qreal zoom);
    void centerChanged(const LonLat& center);
    void viewportChanged(const QSize& viewport);
    
public:
    /**
     * The default constructor requires valid OpenGL context. All initialization
     * goes here.
     *
     * \param parent Passed to the QObject's constructor.
     */
    explicit MapRenderer(QObject* parent = nullptr);
    
    /**
     * The destructor deletes shaders, frees buffers, etc. A valid OpenGL context
     * must be present.
     */
    virtual ~MapRenderer();
    
    WorldTransform transform() const;
    
    /**
     * Gets screen coordinates (0 - winWidth, 0 - winHeight) and
     * maps them to longitude & latitude.
     *
     * \param point The point on the screen.
     * \return Global coordinates (longitude, latitude).
     */
    LonLat mapToLonLat(const QPoint& point);
    
    QPointF scaleToLonLat(const QPoint& vector);
    
    /**
     * Sets the MapDrawer instance.
     * 
     * If another drawer was bound before, it will be deleted.
     * MapRenderer takes ownership over \c drawer.
     * 
     * \note This function is thread-safe.
     */
    void setMapDrawer(MapDrawer* drawer);
    
    qreal zoomStep(int zoom);
    void setZoom(qreal zoom);
    void setCenter(const LonLat& center);
    
    /**
     * The difference between the updateZoom() and setZoom() methods is that
     * updateZoom() updates the zoom property smoothly, i.e. it does some
     * equations that consider the zoom coefficient specified by the user.
     */
    void updateZoom(int steps);

    void setViewport(const QSize& size);
    
    inline const QSize& viewport() const
    {
        return __viewport;
    }
    
    inline qreal zoom() const
    {
        return __zoom;
    }
    inline const LonLat& center() const
    {
        return __center;
    }

    /**
     * Gets the running instance of MapScene.
     */
    inline MapScene* scene()
    {
        return __scene;
    }
    
    inline const QRectF& screen()
    {
        return __screen;
    }

public slots:
    /**
     * Paints the scene using the given painter.
     */
    void paint(QPainter* painter);
    
private:
    void __storeSettings();
    void __restoreSettings();
    void __updateScreen();
    
private:
    /* The current viewport */
    QSize __viewport;
    
    /* Geo coordinates of the current viewport */
    QRectF __screen;
    
    /* The current zoom property */
    qreal __zoom;
    
    /* The center of the map */
    LonLat __center;
    
    /* MapDrawer implementation */
    MapDrawer* __mapDrawer;
    
    /* Scene handler */
    MapScene* __scene;
    
    /* Zoom Coefficient to let users customize their zooming speed */
    /* Zoom Coefficient is defined in MiscellaneousPage */
    
    /* Minimum for __actualZoom not to exceed ZOOM_MAXIMUM value */
    int __actualZoomMaximum;
    
    /*Actual Zoom level*/
    int __actualZoom;
    
};

#endif // MAPRENDERER_H
