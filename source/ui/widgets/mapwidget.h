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

#include <QGLWidget>
#include "vatsimdata/lonlat.h"

class Airport;
class AirportItem;
class FirItem;
class FlightItem;
class Pilot;
class MapItem;
class MapRenderer;
class QMenu;

class MapWidget : public QGLWidget {
  
  Q_OBJECT
  
  friend class MapScene;

signals:
  
  void menuRequest(const MapItem*);
  void menuRequest();
  
  void windowRequest(const MapItem*);
  
public:
  explicit MapWidget(QWidget* = 0);
  virtual ~MapWidget();
  
  bool event(QEvent*) override;
  
  inline MapRenderer* renderer() { return __renderer; }
  
public slots:
  
  /**
   * Closes all the menus, hides tooltip and updates.
   */
  void redraw();
  
protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int, int) override;
  
  void wheelEvent(QWheelEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  
private:
  
  /**
   * Gets item under the mouse cursor. Nullptr if nothing.
   */
  const MapItem* __underMouse();
  
  /**
   * Gets item under the given point. nullptr if nothing.
   */
  const MapItem* __underMouse(const QPoint&);
  
  /**
   * Updates the zoom factor.
   */
  void __updateZoom(int);
  
  /**
   * Creates right-mouse-click menu for an airport.
   */
  QMenu* __itemMenu(const AirportItem*);
  
  /**
   * Creates right-mouse-click menu for a FIR.
   */
  QMenu* __itemMenu(const FirItem*);
  
  /**
   * Creates right-mouse-click menu for a flight.
   */
  QMenu* __itemMenu(const FlightItem*);

private slots:
  void __showMenu(const MapItem*);
  void __showMenu();
  void __showWindow(const MapItem*);
  
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
    void update(const QPoint&);
    
    /**
     * Quickly calculates distance between the given point and mouse cursor
     * position on the screen, _in pixels_.
     */
    qreal screenDistance(const QPoint&);
    
    /**
     * Calculates distance between the point on the globe (lat-lon) and
     * the mouse cursor position.
     */
    qreal geoDistance(const LonLat&);
    
    /**
     * Sets the mouse down status.
     */
    void setDown(bool);
    
    /**
     * Position within the widget.
     */
    inline const QPoint& screenPosition() const { return __screenPosition; }
    
    /**
     * Latitude/longitude that the mouse cursor currently points at.
     */
    inline const LonLat& geoPosition() const { return __geoPosition; }
    
    /**
     * Indicates whether any of the mouse buttons is pressed or not.
     */
    inline bool down() const { return __down; }
    
  private:
    QPoint  __screenPosition;
    LonLat  __geoPosition;
    bool    __down;
  } __mousePosition;
  
  QPoint __lastClickPosition;

};

#endif // MAPWIDGET_H
