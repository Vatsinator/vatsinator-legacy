/*
 * mapwidget.h
 * Copyright (C) 2013-2014  Michał Garapich <michal@garapich.pl>
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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include "ui/map/airportitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/maprenderer.h"
#include "vatsimdata/lonlat.h"

class Client;
class QMenu;
class QGestureEvent;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;

/**
 * A widget that encapsulates MapRenderer in the QWidget.
 */
class MapWidget : public QWidget {
    Q_OBJECT
    friend class MapScene;
    
public:
    /**
     * The default constrcutor passes _parent_ to QGLWidget.
     */
    explicit MapWidget(QWidget* parent = nullptr);
    
    bool event(QEvent* event) override;
    
    inline MapRenderer* renderer()
    {
        return __renderer;
    }
    
protected:
    virtual bool gestureEvent(QGestureEvent* event);
    virtual void pinchTriggered(QPinchGesture* gesture);
    
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    
private:

    /**
     * Gets item under the mouse cursor. Nullptr if nothing.
     */
    const MapItem* __underMouse();
    
    /**
     * Gets item under the given point. nullptr if nothing.
     */
    const MapItem* __underPoint(const QPoint& point);
    
    /**
     * Updates the zoom factor.
     */
    void __updateZoom(int steps);
    
    QMenu* __menuForNoItem();
    QMenu* __menuForAirportItem(const AirportItem* item);
    QString __toolTipForAirportItem(const AirportItem* item);
    QMenu* __menuForFlightItem(const FlightItem* item);
    QString __toolTipForFlightItem(const FlightItem* item);
    QMenu* __menuForFirItem(const FirItem* item);
    QString __toolTipForFirItem(const FirItem* item);
    
private:

    MapRenderer* __renderer;
    
    
    /**
     * This class handles mouse position on the screen.
     */
    class MousePosition {
    public:
        MousePosition();
        
        /**
         * Updates the mouse position from location on the widget (as
         * it is given by Qt).
         */
        void update(const QPoint& pos);
        
        /**
         * Quickly calculates distance between the given point and mouse cursor
         * position on the screen, _in pixels_.
         */
        qreal screenDistance(const QPoint& point);
        
        /**
         * Calculates distance between the point on the globe (lat-lon) and
         * the mouse cursor position.
         */
        qreal geoDistance(const LonLat& point);
        
        /**
         * Sets the mouse down status.
         */
        void setDown(bool down);
        
        /**
         * Position within the widget.
         */
        inline const QPoint& screenPosition() const
        {
            return __screenPosition;
        }
        
        /**
         * Latitude/longitude that the mouse cursor currently points at.
         */
        inline const LonLat& geoPosition() const
        {
            return __geoPosition;
        }
        
        /**
         * Indicates whether any of the mouse buttons is pressed or not.
         */
        inline bool down() const
        {
            return __down;
        }
        
    private:
        QPoint  __screenPosition;
        LonLat  __geoPosition;
        bool    __down;
    } __mousePosition;
    
    QPoint __lastClickPosition;
    
};

#endif // MAPWIDGET_H
